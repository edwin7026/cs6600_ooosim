/**
 * @file types.h
 * @details This file contains definitions of basic structs and enums
 * @author Edwin Joy <edwin7026@gmail.com>
 */

#ifndef TYPES_H
#define TYPES_H

#include <iostream>
#include <string>
#include <utility>

/**
 * @details enum for all stages
 */
enum stage {
    FETCH,
    DISPATCH,
    ISSUE,
    EXECUTE,
    WRITEBACK,
    COMMIT
};

/**
 * @details This struct holds the info from trace file
 */
struct if_packet {
    unsigned pc;
    std::string instr;

    // initialization
    if_packet() : pc(0), instr("NA") {}
};

/**
 * @details This struct holds content of rob element
 */
struct rob_elem {
    unsigned pc;
    int tag;
    unsigned fu_type;
    stage inst_stage;
    
    // register with its RDY value
    std::pair<bool, int> rs1;
    std::pair<bool, int> rs2;
    int rd;

    int rs1_abs;
    int rs2_abs;

    // is ready for execution
    bool is_rdy;

    // timing spec
    unsigned if_start;
    unsigned if_num_cyc;
    unsigned id_start;
    unsigned id_num_cyc;
    unsigned is_start;
    unsigned is_num_cyc;
    unsigned ex_start;
    unsigned ex_num_cyc;
    unsigned wb_start;
    unsigned wb_num_cyc;
    
    // constructor
    rob_elem()
    {   
        pc = 0;
        tag = 0;
        fu_type = 0;
        inst_stage = stage::FETCH;

        // initialize registers
        rs1.first = false;
        rs1.second = -1;
        rs2.first = false;
        rs2.second = -1;
        rd = -1;

        // logic to see if ready to be issued
        is_rdy = false;

        // initialize counters
        if_start = 0;
        if_num_cyc = 0;
        id_start = 0;
        id_num_cyc = 0;
        is_start = 0;
        is_num_cyc = 0;
        ex_start = 0;
        ex_num_cyc = 0;
        wb_start = 0;
        wb_num_cyc = 0;
    }

    /**
     * @details Pretty print instruction state
     */
    const std::string pprint() {
        return std::to_string(tag) + 
                "  fu{" + std::to_string(fu_type) + "}" +
                " src{" + std::to_string(rs1_abs) + "," + std::to_string(rs2_abs) + "}" +
                " dst{" + std::to_string(rd) + "}" +
                " IF{" + std::to_string(if_start) + "," + std::to_string(if_num_cyc) + "}" +
                " ID{" + std::to_string(id_start) + "," + std::to_string(id_num_cyc) + "}" +
                " IS{" + std::to_string(is_start) + "," + std::to_string(is_num_cyc) + "}" +
                " EX{" + std::to_string(ex_start) + "," + std::to_string(ex_num_cyc) + "}" +
                " WB{" + std::to_string(wb_start) + "," + std::to_string(wb_num_cyc) + "}";
    }
};

#endif // TYPES_H