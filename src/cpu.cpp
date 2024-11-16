/**
 * @file cpu.cpp
 * @details This file contains definitions for cpu class
 * @author Edwin Joy <edwin7026@gmail.com>
 */

#include "cpu.h"

cpu::cpu(unsigned if_bandwidth, unsigned sched_q_size, const std::string& trace_file_path, const logger& log) : 
    base("CPU"),
    _log(log),
    _dollar_finish(false),
    _if_bandwidth(if_bandwidth),
    _sched_q_size(sched_q_size),
    _tag(0),
    _exec_queue(if_bandwidth),
    _instr_stream(trace_file_path)
{
    _log.log(this, verbose::DEBUG, "Constructing CPU");

    // initialize regfile to ready with tag = 0 on reset
    for (auto& each : _reg_file) {
        each.first = true;
        each.second = 0x0;
    }
}

bool cpu::advance_and_check() {
    
    // print rob content on every cycle
    // for(auto& each : _rob) {
    //     std::cout << "Tag: " << each->tag << " STAGE: " << each->inst_stage << std::endl;
    // }
    // std::cout << "RDY: " << _reg_file[14].first << " Tag: " << _reg_file[14].second << std::endl;

    // cycle incrementation
    _cycle_count = _cycle_count + 1;

    // DEBUG
    if (_cycle_count > 2000)
        return false;
    return true;
    
    // advance cycle until rob is not empty
    return !_rob.empty();
}

void cpu::fetch() 
{
    _log.log(this, verbose::DEBUG, "FETCH :: Entering fetch stage");

    ////////// Send out instructions to next stage subject to conditions //////////

    // advance instructions in IF to ID stage subject to size of ID (max N in a cycle)
    unsigned count = 0;
    for (auto& each : _rob)
    {
        // check if ID queue is full or max N instructions are selected
        // if so break
        if ((_id_queue.size() == 2*_if_bandwidth) || (count == _if_bandwidth)) {
            break;
        }
        
        // if instruction is in fetch stage push them to ID subject to above conditions
        if (each->inst_stage == stage::FETCH) {
            // enter cycle parameters before leaving
            // elapsed number of cycles
            each->if_num_cyc = _cycle_count - each->if_start;

            // log the leaving instruction
            _log.log(this, verbose::DEBUG, "FETCH :: Leaving: " + each->pprint());

            // change state of instruction from IF to ID
            each->inst_stage = stage::DISPATCH;
            // add start cycle when instruction was passed to DISPATCH
            each->id_start = _cycle_count;

            // emplace into the next queue
            _id_queue.emplace_back(each);

            // increment count
            count = count + 1;
        }
    }

    _log.log(this, verbose::DEBUG, "FETCH :: Advancing " + std::to_string(count) + " instructions to ID");

    ////////// Bring in instructions to IF stage subject to condtions //////////

    // take the minimum of quantities: a. the IF bandwidth or the amount of space in ID queue
    // TODO not sure if this is right
    unsigned num_instr_to_fetch = std::min(_if_bandwidth, 2*_if_bandwidth - static_cast<unsigned>(_id_queue.size()));

    _log.log(this, verbose::DEBUG, "FETCH :: Computed number of instructions to fetch: " + std::to_string(num_instr_to_fetch));

    // temp variables
    std::string line;
    std::string opr;

    // fetch
    if(_instr_stream.is_open())
    {
        for(unsigned i = 0; i < num_instr_to_fetch; i++)
        {
            getline(_instr_stream, line);

            std::stringstream ss(line);

            rob_elem* temp_rob_elem_ptr = new rob_elem();
            
            try
            {
                temp_rob_elem_ptr->tag = _tag;
                _tag = _tag + 1;

                ss >> opr;
                // get the pc
                temp_rob_elem_ptr->pc = std::stoul(opr, nullptr, 16);

                ss >> opr;
                // get fu type
                temp_rob_elem_ptr->fu_type = std::stoul(opr);

                ss >> opr;
                // dest reg
                temp_rob_elem_ptr->rd = std::stoi(opr);

                ss >> opr;
                temp_rob_elem_ptr->rs1.second = std::stoi(opr);
                temp_rob_elem_ptr->rs1_abs = temp_rob_elem_ptr->rs1.second;
                ss >> opr;
                temp_rob_elem_ptr->rs2.second = std::stoi(opr);
                temp_rob_elem_ptr->rs2_abs = temp_rob_elem_ptr->rs2.second;

                // set stage
                temp_rob_elem_ptr->inst_stage = stage::FETCH;

                // set cycle params of the stage
                temp_rob_elem_ptr->if_start = _cycle_count;

                // log the line
                _log.log(this, verbose::DEBUG, "FETCH :: Adding to ROB: " + temp_rob_elem_ptr->pprint());
            
                // insert element to the rob datastructure
                _rob.emplace_back(temp_rob_elem_ptr);
            }
            catch (std::invalid_argument&) {
                _log.log(this, verbose::DEBUG, "Invalid line");
            }

            // if we reach end of file, break
            if (_instr_stream.eof()) {
                _dollar_finish = true;
                break;
            }
        }
    }
    else {
        _log.log(this, verbose::FATAL, "FETCH :: Unable to find file");
        exit(1);
    }
}

void cpu::dispatch() 
{
    _log.log(this, verbose::DEBUG, "DISPATCH :: Entering dispatch stage");

    ////////// Send out instructions to next stage subject to conditions //////////

    // advance instructions from ID to IS
    unsigned count = 0;
    // go through dispatch queue
    // push the instruction to IS stage until scheduling queue is full or number of instructions
    // selected to be sent is if_bandwidth 
    for (auto& each : _rob)
    {
        // choose only the dispatch ones 
        if (each->inst_stage != stage::DISPATCH) {
            continue;
        }

        // check if IS queue is full or max N instructions are selected
        if ((_sched_queue.size() == _sched_q_size) || (count == _if_bandwidth)) {
            break;
        }

        // enter cycle parameters before leaving
        // elapsed number of cycles
        each->id_num_cyc = _cycle_count - each->id_start;

        // log the leaving instruction
        _log.log(this, verbose::DEBUG, "DISPATCH :: Leaving: " + each->pprint());

        // change state of instruction from ID to IS
        each->inst_stage = stage::ISSUE;
        
        // enter start cycle for issue
        each->is_start = _cycle_count;

        // renaming of source and dest if busy happens here
        rename(each);

        // emplace into scheduler
        _sched_queue.emplace_back(each);

        // delete from the ID queue
        _id_queue.remove(each);

        // increment count
        count = count + 1;
    }
    _log.log(this, verbose::DEBUG, "DISPATCH :: Advancing " + std::to_string(count) + " instructions to IS");
}

void cpu::issue()
{
    _log.log(this, verbose::DEBUG, "ISSUE :: Entering issue stage");

    ////////// Send out instructions to next stage subject to conditions //////////

    // advance instructions from IS to EX
    unsigned count = 0;
    for (auto& each : _rob)
    {
        // if inst not in IS stage, continue
        if (each->inst_stage != stage::ISSUE) {
            continue;
        }

        // update the readiness of instruction 
        update_ready(each);

        // if instruction is not ready for execution
        if (!each->is_rdy) {
            continue;
        }

        if (count == _if_bandwidth) {
            break;
        }

        // enter cycle parameters before leaving
        // elapsed number of cycles
        each->is_num_cyc = _cycle_count - each->is_start;

        // log the leaving instruction
        _log.log(this, verbose::DEBUG, "ISSUE :: Leaving: " + each->pprint());

        // change state of instruction from IS to EX
        each->inst_stage = stage::EXECUTE;

        // enter cycle start for EX;
        each->ex_start = _cycle_count;

        // emplace into next queue
        _exec_queue.emplace_back(each);

        // delete from the IS queue
        _sched_queue.remove(each);

        // increment count
        count = count + 1;
    }
    _log.log(this, verbose::DEBUG, "ISSUE :: Advancing " + std::to_string(count) + " instructions to EX stage");
}

void cpu::execute()
{
    _log.log(this, verbose::DEBUG, "EXECUTE :: Entering execute stage");
    
    ////////// Send out instructions to next stage subject to conditions //////////

    // advance instructions from EX to WB
    for(auto& each : _rob) 
    {
        if (each->inst_stage != stage::EXECUTE) {
            continue;
        }

        // if execution is complete
        if (advance_exec(each)) 
        {
            _log.log(this, verbose::DEBUG, "EX :: Leaving: " + each->pprint());
            each->inst_stage = stage::WRITEBACK;
            each->wb_start = _cycle_count;
            _exec_queue.remove(each);
        }
    }
}

void cpu::retire() 
{
    _log.log(this, verbose::DEBUG, "RETIRE :: Entering retire stage");

    //std::list<rob_elem*> purge_lst;
    for (auto& each : _rob)
    {
        if(each->inst_stage != stage::WRITEBACK) {
            continue;
        }
        each->inst_stage = stage::COMMIT;
        
        // add cycle count
        each->wb_num_cyc = _cycle_count - each->wb_start;
        _log.log(this, verbose::DEBUG, "Commit :: " + each->pprint());

        //purge_lst.emplace_back(each);
    }
    
    // remove from rob
    //for(auto& each : purge_lst) {
        //_rob.remove(each);
        //delete each;
    //}
}

void cpu::simulate()
{
    do {
        retire();
        execute();
        issue();
        dispatch();
        fetch();
    } while(advance_and_check()); // add conditions for end of trace

    print_rob();
}

void cpu::rename(rob_elem *inst)
{
    // if ready in regfile, set instr reg as ready
    // else copy the tag of busy register to the rob element reg
    if (inst->rs1.second != -1) {
        // if ready
        if (_reg_file[inst->rs1.second].first) {
            inst->rs1.first = true;
        } else {
            inst->rs1.first = false;
            inst->rs1.second = _reg_file[inst->rs1.second].second;
        }
    } else {
        inst->rs1.first = true;
    }

    if (inst->rs2.second != -1) {
        // if ready
        if (_reg_file[inst->rs2.second].first) {
            inst->rs2.first = true;
        } else {
            inst->rs2.first = false;
            inst->rs2.second = _reg_file[inst->rs2.second].second;
        }
    } else {
        inst->rs2.first = true;
    }

    // set destination register as not ready in regfile
    // add tag of this instruction to the regfile
    // NOTE: this should only happen if rs != rd
    if (inst->rd != -1) {
        _reg_file[inst->rd].first = false;
        _reg_file[inst->rd].second = inst->tag;
    }
}

void cpu::update_ready(rob_elem* inst)
{
    if(inst->rs1.first && inst->rs2.first) {
        inst->is_rdy = true;
        return;
    }

    // update
    else {
        // update from register file

        // for rs1
        if (!inst->rs1.first) {
            // check if register is ready
            for(auto& reg : _reg_file) {
                // check if same tag withthe producer
                if (reg.second == inst->rs1.second) {
                    if (reg.first) {
                        inst->rs1.first = true;
                    }
                    break;
                }
            }
        }

        // for rs2
        if (!inst->rs2.first) {
            // check if register is ready
            for(auto& reg : _reg_file) {
                // check if same tag with producer
                if (reg.second == inst->rs2.second) {
                    if (reg.first) {
                        inst->rs2.first = true;
                    }
                    break;
                }
            }
        }
    }

    // finally, update the status
    if(inst->rs1.first && inst->rs2.first) {
        inst->is_rdy = true;
    }
}


bool cpu::advance_exec(rob_elem *inst)
{
    unsigned fu_type = inst->fu_type;
    unsigned ex_cycles = 0;
    // get num cycles
    switch(fu_type){
        case 0 : ex_cycles = 1; break;
        case 1 : ex_cycles = 2; break;
        case 2 : ex_cycles = 10; break;
    }
    // get num cycles elapsed 
    inst->ex_num_cyc = _cycle_count - inst->ex_start;
    if (inst->ex_num_cyc == ex_cycles) 
    {
        if (inst->rd != -1)
        {
            if (_reg_file[inst->rd].second == inst->tag) {
                _reg_file[inst->rd].first = true;
            }

            // wake up instructions
            for (auto& each : _sched_queue)
            {
                if(!each->rs1.first) {
                    if (each->rs1.second == inst->tag) {
                       each->rs1.first = true; 
                    }
                }
                if(!each->rs2.first) {
                    if (each->rs2.second == inst->tag) {
                       each->rs2.first = true; 
                    }
                }
            }
        }
        return true;
    } else {
        return false;
    }
}

void cpu::print_rob()
{
    for(auto& inst : _rob) {
        std::cout << inst->pprint() << std::endl; 
    }
}
