# This code should be functional. Doesn't have to be optimal.
# I'm writing it to prove that it can be done.

#include "riscv/encoding.h"

# TODO: Update these constants once they're finalized in the doc.

#define DCSR                    0x790
#define DCSR_CAUSE_DEBINT       3
#define DCSR_HALT_OFFSET        3
#define DCSR_DEBUGINT_OFFSET    10

#define DSCRATCH                0x792

# TODO: Should be 0x400
#define DEBUG_RAM               (-0x400)
#define DEBUG_RAM_SIZE          64

# TODO: Should be 0x100, 0x108
#define SETHALTNOT              (-0x100)
#define CLEARDEBINT             (-0x108)

        .global entry
        .global resume

        # Automatically called when Debug Mode is first entered.
entry:  j       _entry
        # Should be called by Debug RAM code that has finished execution and
        # wants to return to Debug Mode.
resume:
        # Clear debug interrupt.
clear_debint:
        csrr    s1, CSR_MHARTID
        sw      s1, CLEARDEBINT(zero)
        fence

        # Restore s1.
        csrr    s1, CSR_MISA
        bltz    s1, restore_not_32
restore_32:
        lw      s1, (DEBUG_RAM + DEBUG_RAM_SIZE - 4)(zero)
        j       check_halt
restore_not_32:
        slli    s1, s1, 1
        bltz    s1, restore_128
restore_64:
        ld      s1, (DEBUG_RAM + DEBUG_RAM_SIZE - 8)(zero)
        j       check_halt
restore_128:
        nop     #lq      s1, (DEBUG_RAM + DEBUG_RAM_SIZE - 16)(zero)

check_halt:
        csrr    s0, DCSR
        andi    s0, s0, (1<<DCSR_HALT_OFFSET)
        beqz    s0, exit
        j       wait_for_interrupt

exit:
        # Restore s0.
        csrr    s0, DSCRATCH
        eret


_entry:
        # Save s0 in DSCRATCH
        csrw    DSCRATCH, s0

        # Check why we're here
        csrr    s0, DCSR
        # cause is in bits 2:0 of dcsr
        andi    s0, s0, 7
        addi    s0, s0, -DCSR_CAUSE_DEBINT
        bnez    s0, spontaneous_halt

jdebugram:
        # Save s1 so that the debug program can use two registers.
        csrr    s0, CSR_MISA
        bltz    s0, save_not_32
save_32:
        sw      s1, (DEBUG_RAM + DEBUG_RAM_SIZE - 4)(zero)
        jr      zero, DEBUG_RAM
save_not_32:
        slli    s0, s0, 1
        bltz    s0, save_128
save_64:
        sd      s1, (DEBUG_RAM + DEBUG_RAM_SIZE - 8)(zero)
        jr      zero, DEBUG_RAM
save_128:
        nop     #sq      s1, (DEBUG_RAM + DEBUG_RAM_SIZE - 16)(zero)
        jr      zero, DEBUG_RAM

spontaneous_halt:
        csrr    s0, CSR_MHARTID
        sw      s0, SETHALTNOT(zero)
        csrsi   DCSR, (1<<DCSR_HALT_OFFSET)

wait_for_interrupt:
        csrr    s0, DCSR
        andi    s0, s0, (1<<DCSR_DEBUGINT_OFFSET)
        beqz    s0, wait_for_interrupt

        j       jdebugram
