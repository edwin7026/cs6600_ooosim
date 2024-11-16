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
#include <array>

// local includes
#include <common.h>
#include <types.h>

/**
 * @details This class mimics the instruction fetch stage of a cpu
 */
class cpu : base
{
    private:
        // logger reference 
        logger _log;

        // parameters
        unsigned _if_bandwidth;
        unsigned _sched_q_size;

        unsigned _tag;

        // data structures
        std::array<std::pair<bool, int>, 128> _reg_file;

        std::list<rob_elem*> _rob;
        std::list<rob_elem*> _id_queue;
        std::list<rob_elem*> _sched_queue;
        std::list<rob_elem*> _exec_queue;

        // memory interfacing
        std::ifstream _instr_stream;
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

        void rename(rob_elem* inst);
        void advance_exec(rob_elem* inst); 
};

#endif // CPU_H