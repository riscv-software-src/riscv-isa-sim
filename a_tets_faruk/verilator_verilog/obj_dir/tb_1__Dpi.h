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
    // DPI import at src/rtl/tb/tb_1.sv:47:30
    extern int test_c(int operand1, int operand2);

#ifdef __cplusplus
}
#endif

#endif  // guard
