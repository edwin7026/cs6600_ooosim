/**
 * @file cpu.h
 * @details Top level CPU description
 * @author Edwin Joy <edwin7026@gmail.com>
 */

#ifndef CPU_H
#define CPU_H

// standard includes
#include <string>
#include <fstream>
#include <sstream>
#include <list>

// local includes
#include <common.h>
#include <types.h>

/**
 * @details This class mimics the instruction fetch stage of a cpu
 */
class cpu : base
{
    private:
        // parameters
        unsigned _if_bandwidth;
        unsigned _sched_q_size;

        // data structures
        std::list<rob_elem> _rob;
        std::list<rob_elem> _id_queue;

        // signals and control
        unsigned _cycle_count;

        // memory interfacing
        std::ifstream _instr_stream;

        // logger reference 
        logger _log;
    public:
        /**
         * @details Constructor that consumes the parametes for the cpu and interfaces
         * instruction memory
         * @param if_bandwidth Instruction Fetch bandwidth
         * @param sched_q_size Scheduler queue size
         * @param trace_path Path to the trace input file
         * @param logger Logger instance
         */
        cpu(unsigned if_bandwidth, unsigned sched_q_size, const std::string& trace_path, const logger& log);

        /**
         * @details advance simulation cycle
         */
        bool advance_and_check();

        /**
         * @details This function runs a fetch method subject to constraints
         */
        void fetch();

        void dispatch();
        void issue();
        void execute();
        void retire();

        /**
         * @details Simulate the core
         */
        void simulate();
};

#endif // CPU_H