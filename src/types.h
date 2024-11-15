/**
 * @file cpu.h
 * @details This file contains the class declaration for IF stage of CPU
 * @author Edwin Joy <edwin7026@gmail.com>
 */

#ifndef TYPES_H
#define TYPES_H

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
    std::pair<int, int> source_reg;
    int dest_reg;

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
};

#endif // TYPES_H