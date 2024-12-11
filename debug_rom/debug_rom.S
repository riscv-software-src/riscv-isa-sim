// See LICENSE.SiFive for license details.

#include "riscv/encoding.h"
#include "riscv/debug_rom_defines.h"

        .option norvc
        .global entry
        .global exception

// This macro handles mem access with proper management of the MPRVEN
// Usage: MEMORY_ACCESS_WITH_MPRV(<your code>)
#define MEMORY_ACCESS_WITH_MPRV(...)       \
       csrrci s0, CSR_DCSR, DCSR_MPRVEN;   \
       andi   s0, s0, DCSR_MPRVEN;         \
       bnez   s0, 1f;                      \
       __VA_ARGS__;                        \
       j      2f;                          \
1:                                         \
       __VA_ARGS__;                        \
       csrrsi zero, CSR_DCSR, DCSR_MPRVEN; \
2:

        // Entry location on ebreak, Halt, or Breakpoint
        // It is the same for all harts. They branch when 
        // their GO or RESUME bit is set.

entry:
       jal zero, _entry
resume:
       // Not used.
       jal zero, _resume
exception:
       jal zero, _exception

_entry:
        // This fence is required because the execution may have written something
        // into the Abstract Data or Program Buffer registers.
        fence
        csrw CSR_DSCRATCH0, s0  // Save s0 to allow signaling MHARTID

        // We continue to let the hart know that we are halted in order that
        // a DM which was reset is still made aware that a hart is halted.
        // We keep checking both whether there is something the debugger wants
        // us to do, or whether we should resume.
entry_loop:
        // 1 byte flag per hart. Only one hart advances here.
        MEMORY_ACCESS_WITH_MPRV(
          csrr s0, CSR_MHARTID;
          sw   s0, DEBUG_ROM_HALTED(zero);
          lbu  s0, DEBUG_ROM_FLAGS(s0);
        )

        andi s0, s0, (1 << DEBUG_ROM_FLAG_GO)
        bnez s0, going

        // multiple harts can resume  here
        MEMORY_ACCESS_WITH_MPRV(
          csrr s0, CSR_MHARTID; 
          lbu  s0, DEBUG_ROM_FLAGS(s0);
        )

        andi s0, s0, (1 << DEBUG_ROM_FLAG_RESUME)
        bnez s0, _resume
        wfi
        jal  zero, entry_loop

_exception:
        // Restore S0, which we always save to dscratch.
        // We need this in case the user tried an abstract write to a
        // non-existent CSR.


        // Let debug module know you got an exception.
        MEMORY_ACCESS_WITH_MPRV(
          csrr s0, CSR_DSCRATCH0; 
          sw zero, DEBUG_ROM_EXCEPTION(zero);
        )

        ebreak

going:
        // When debug module sees this write, the GO flag is reset.
        MEMORY_ACCESS_WITH_MPRV(
          csrr s0, CSR_MHARTID; 
          sw s0, DEBUG_ROM_GOING(zero);
        )

        csrr s0, CSR_DSCRATCH0            // Restore s0 here
        fence
        fence.i
        jalr zero, zero, %lo(whereto)    // Debug module will put different instructions and data in the RAM, 
                                         // so we use fence and fence.i for safety. (rocket-chip doesn't have this
                                         // because jalr is special there)

_resume:
        // When Debug Module sees this write, the RESUME flag is reset.
        MEMORY_ACCESS_WITH_MPRV(
          csrr s0, CSR_MHARTID; 
          sw   s0, DEBUG_ROM_RESUMING(zero);
        )

        csrr s0, CSR_DSCRATCH0   // Restore s0
        dret

        // END OF ACTUAL "ROM" CONTENTS. BELOW IS JUST FOR LINKER SCRIPT.

.section .whereto
whereto:
        nop
        // Variable "ROM" This is : jal x0 abstract, jal x0 program_buffer,
        //                or jal x0 resume, as desired.
        //                Debug Module state machine tracks what is 'desired'.
        //                We don't need/want to use jalr here because all of the
        //                Variable ROM contents are set by
        //                Debug Module before setting the OK_GO byte.
