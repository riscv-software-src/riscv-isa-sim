// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Prototypes for DPI import and export functions.
//
// Verilator includes this file in all generated .cpp files that use DPI functions.
// Manually include this file where DPI .c import functions are declared to ensure
// the C functions match the expectations of the DPI imports.

#ifndef VERILATED_TB_3__DPI_H_
#define VERILATED_TB_3__DPI_H_  // guard

#include "svdpi.h"

#ifdef __cplusplus
extern "C" {
#endif


    // DPI IMPORTS
    // DPI import at src/rtl/tb/tb_3.sv:48:32
    extern void c_init_dynamic(const svOpenArrayHandle src1, const svOpenArrayHandle src2);
    // DPI import at src/rtl/tb/tb_3.sv:46:31
    extern int get_last_commit();
    // DPI import at src/rtl/tb/tb_3.sv:47:32
    extern void step_dynamic();

#ifdef __cplusplus
}
#endif

#endif  // guard
