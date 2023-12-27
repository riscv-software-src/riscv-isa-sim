// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Prototypes for DPI import and export functions.
//
// Verilator includes this file in all generated .cpp files that use DPI functions.
// Manually include this file where DPI .c import functions are declared to ensure
// the C functions match the expectations of the DPI imports.

#ifndef VERILATED_TB_4__DPI_H_
#define VERILATED_TB_4__DPI_H_  // guard

#include "svdpi.h"

#ifdef __cplusplus
extern "C" {
#endif


    // DPI IMPORTS
    // DPI import at src/rtl/tb/tb_4.sv:46:31
    extern int get_last_commit();
    // DPI import at src/rtl/tb/tb_4.sv:48:32
    extern void init(const svOpenArrayHandle target1, const svOpenArrayHandle target2);
    // DPI import at src/rtl/tb/tb_4.sv:47:32
    extern void step_glob();

#ifdef __cplusplus
}
#endif

#endif  // guard
