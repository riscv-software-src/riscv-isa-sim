// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Prototypes for DPI import and export functions.
//
// Verilator includes this file in all generated .cpp files that use DPI functions.
// Manually include this file where DPI .c import functions are declared to ensure
// the C functions match the expectations of the DPI imports.

#ifndef VERILATED_TB_ASSOC_ARR__DPI_H_
#define VERILATED_TB_ASSOC_ARR__DPI_H_  // guard

#include "svdpi.h"

#ifdef __cplusplus
extern "C" {
#endif


    // DPI IMPORTS
    // DPI import at src/rtl/tb/tb_assoc_arr.sv:29:32
    extern void write_unordered_map_to_sv_open_arrays(const svOpenArrayHandle key_array, const svOpenArrayHandle value_array, int* num_elements_inserted);

#ifdef __cplusplus
}
#endif

#endif  // guard
