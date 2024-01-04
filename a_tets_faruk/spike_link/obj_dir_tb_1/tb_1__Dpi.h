// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Prototypes for DPI import and export functions.
//
// Verilator includes this file in all generated .cpp files that use DPI functions.
// Manually include this file where DPI .c import functions are declared to ensure
// the C functions match the expectations of the DPI imports.

#ifndef VERILATED_TB_1__DPI_H_
#define VERILATED_TB_1__DPI_H_  // guard

#include "svdpi.h"

#ifdef __cplusplus
extern "C" {
#endif


    // DPI IMPORTS
    // DPI import at src/rtl/tb/tb_1.sv:39:32
    extern void clear_last_commit();
    // DPI import at src/rtl/tb/tb_1.sv:32:32
    extern void get_last_commit(const svOpenArrayHandle key_array, const svOpenArrayHandle value_array, int* num_elements_inserted);
    // DPI import at src/rtl/tb/tb_1.sv:26:31
    extern int init(int argc, const svOpenArrayHandle argv);
    // DPI import at src/rtl/tb/tb_1.sv:29:32
    extern void step();
    // DPI import at src/rtl/tb/tb_1.sv:19:32
    extern void write_unordered_map_to_sv_open_arrays(const svOpenArrayHandle key_array, const svOpenArrayHandle value_array, int* num_elements_inserted);

#ifdef __cplusplus
}
#endif

#endif  // guard
