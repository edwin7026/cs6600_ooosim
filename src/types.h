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
    WRITEBACK
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
    unsigned tag;
    unsigned fu_type;
    stage inst_stage;
    
    // register with its RDY value
    std::pair<bool, int> rs1;
    std::pair<bool, int> rs2;
    int rd;

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
        // initialize registers
        rs1.first = false;
        rs1.second = -1;
        rs2.first = false;
        rs2.second = -1;
        rd = -1;

        is_rdy = false;
    }

    /**
     * @details Pretty print instruction state
     */
    const std::string pprint() {
        return std::to_string(tag) + 
                "  fu{" + std::to_string(fu_type) + "}" +
                " src{" + std::to_string(rs1.second) + "," + std::to_string(rs2.second) + "}" +
                " dst{" + std::to_string(rd) + "}" +
                " IF{" + std::to_string(if_start) + "," + std::to_string(if_num_cyc) + "}" +
                " ID{" + std::to_string(id_start) + "," + std::to_string(id_num_cyc) + "}";
    }
};

#endif // TYPES_H