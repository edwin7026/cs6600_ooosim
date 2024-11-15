/**
 * @file cpu.cpp
 * @details This file contains definitions for cpu class
 * @author Edwin Joy <edwin7026@gmail.com>
 */

#include "cpu.h"

cpu::cpu(unsigned if_bandwidth, unsigned sched_q_size, const std::string& trace_file_path, const logger& log) : 
    base("CPU"),
    _log(log),
    _if_bandwidth(if_bandwidth),
    _sched_q_size(sched_q_size),
    _tag(0),
    _instr_stream(trace_file_path)
{
    _log.log(this, verbose::DEBUG, "Constructing CPU");
    
    _cycle_count = 0;
}

bool cpu::advance_and_check() {
    // cycle incrementation
    _cycle_count = _cycle_count + 1;
    
    // check if ifstream points to EOF
    return _instr_stream.eof() == false;
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
        if (_id_queue.size() == 2*_if_bandwidth || count == 5) {
            break;
        }
        
        // if instruction is in fetch stage push them to ID subject to above conditions
        if (each->inst_stage == stage::FETCH) {
            _id_queue.emplace_back(each);
            count = count + 1;
        }
    }

    ////////// Bring in instructions to IF stage subject to condtions //////////

    // take the minimum of quantities: a. the IF bandwidth or the amount of space in ID queue
    unsigned num_instr_to_fetch = std::min(_if_bandwidth, 2*_if_bandwidth - static_cast<unsigned>(_id_queue.size()));

    _log.log(this, verbose::DEBUG, "FETCH :: Computed number of instructions to fetch: " + std::to_string(num_instr_to_fetch));

    // temp variables
    std::string line;
    std::string opr;

    // fetch
    if(_instr_stream.is_open())
    {
        for(unsigned i = 0; i < num_instr_to_fetch && (!_instr_stream.eof()); i++)
        {
            getline(_instr_stream, line);

            _log.log(this, verbose::DEBUG, "Reading line: " + line);

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
                temp_rob_elem_ptr->dest_reg = std::stoi(opr);

                ss >> opr;
                temp_rob_elem_ptr->source_reg.first = std::stoi(opr);
                ss >> opr;
                temp_rob_elem_ptr->source_reg.second = std::stoi(opr);

                // set stage
                temp_rob_elem_ptr->inst_stage = stage::FETCH;
            
                // insert element to the rob datastructure
                _rob.emplace_back(temp_rob_elem_ptr);
            }
            catch (std::invalid_argument&) {
                _log.log(this, verbose::DEBUG, "Invalid line");
            }
        }
    }
    else {
        _log.log(this, verbose::FATAL, "FETCH :: Unable to find file");
        exit(1);
    }

    if (_tag >= 3) {
        exit(0);
    }
}

void cpu::dispatch() 
{
    _log.log(this, verbose::DEBUG, "DISPATCH :: Entering dispatch stage");

    // advance instructions from ID to IS

    // renaming of source and destination

    // check how much to send to issue stage
}

void cpu::issue() {

    // advance instructions from IS to EX

    // add ID instructions to issue subject to space left in queue

    // priority is based on tag value
}

void cpu::execute() {
    // advance instructions from EX to restire
}

void cpu::retire() {

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
}