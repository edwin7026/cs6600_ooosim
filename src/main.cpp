/**
 * @file main.cpp
 * @details This file has the main function definition
 * @author Edwin Joy <edwin7026@gmail.com>
 */

// standard imports
#include <iostream>
#include <string>

// local imports
#include <cpu.h>
#include <common.h>

int main(int argc, char* argv[])
{
    // initialize logger
    logger log(verbose::INFO);

    log.log(nullptr, verbose::DEBUG, "Parsing arguments...");

    // gather arguments    
    unsigned super_bandwidth = std::stoul(argv[1]);
    unsigned sched_q_size = std::stoul(argv[2]);
    std::string trace_file_path = argv[3];

    // instantiate cpu with given params and interface instruction memory
    cpu cpu_inst(super_bandwidth, sched_q_size, trace_file_path, log);

    // simulate cpu
    cpu_inst.simulate();
    
    // results
    cpu_inst.print_config();
    cpu_inst.print_results();

    return 0;
}