/**
 * @file cpu.cpp
 * @details This file contains definitions for cpu class
 * @author Edwin Joy <edwin7026@gmail.com>
 */

#include "cpu.h"

cpu::cpu(unsigned if_bandwidth, unsigned sched_q_size, const std::string& trace_file_path, const logger &log) : 
    base("CPU"),
    _instr_stream(trace_file_path)
{
    _log.log(this, verbose::DEBUG, "Constructing CPU");
    _if_bandwidth = if_bandwidth;
    _sched_q_size = sched_q_size;
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
    // TODO
    // advance instructions in IF to ID stage

    // figure out how many to fetch
    // TOOD now it just fetches equal to bandwidth number of instructions
    unsigned num_instr_to_fetch = _if_bandwidth;

    // temp variables
    std::string line;
    std::string opr;
    rob_elem temp_rob_elem;

    // fetch
    if(_instr_stream.is_open())
    {
        for(unsigned i = 0; i < num_instr_to_fetch; i++)
        {
            getline(_instr_stream, line);

            _log.log(this, verbose::DEBUG, "Reading line: " + line);

            std::stringstream ss(line);

            ss >> opr;
            // get the pc
            temp_rob_elem.pc = std::stoul(opr, (std::size_t*)0, 16);

            ss >> opr;
            // get fu type
            temp_rob_elem.fu_type = std::stoul(opr);

            ss >> opr;
            // dest reg
            temp_rob_elem.dest_reg = std::stoi(opr);

            ss >> opr;
            temp_rob_elem.source_reg.first = std::stoi(opr);
            ss >> opr;
            temp_rob_elem.source_reg.second = std::stoi(opr);
            
            // insert element to the rob datastructure
            _rob.emplace_back(temp_rob_elem);
        }
    }
    else {
        _log.log(this, verbose::FATAL, "FETCH :: Unable to find file");
        exit(1);
    }

    // set all instructions to IF stage
}

void cpu::dispatch() {

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