#include "../riscv/encoding.h"
#define PGSHIFT         12

        .global         _start

        .section        .text
_start:
        # Set up a page table entry that maps 0x0... to 0x8...
        la      t0, page_table
        srli    t0, t0, PGSHIFT
        csrw    CSR_SPTBR, t0

        # update mstatus
        csrr    t1, CSR_MSTATUS
        li      t0, (MSTATUS_MPRV | (VM_SV39 << 24))
        #li      t0, ((VM_SV39 << 24))
        or      t1, t0, t1
        csrw    CSR_MSTATUS, t1

        la      t0, (loop - 0x80000000)
        csrw    CSR_MEPC, t0

        # Exit supervisor mode, entering user mode at loop.
        mret

loop:
        la      t0, data
        lw      t1, 0(t0)
        j       loop

        .section        .data
data:
        .word   0xbead

        .balign 0x1000
page_table:
        .word   ((0x80000000 >> 2) | PTE_V | PTE_U | PTE_R | PTE_W | PTE_X)
        .word   0
