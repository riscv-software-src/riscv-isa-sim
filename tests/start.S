.section .text
.globl _start

_start:
    la sp, _stack_top

    # set mtvec = trap_entry
    la t0, trap_entry
    csrw mtvec, t0

    call main

1:  j 1b

.align 2
trap_entry:
    # If we trapped on ECALL, just skip the instruction and return.
    csrr t0, mepc
    addi t0, t0, 4          # ecall is 4 bytes
    csrw mepc, t0
    mret
