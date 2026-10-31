// See LICENSE for license details.

// ebreak_fetch_fault_test
//  Test for ebreak fetch fault handling in RISC-V semihosting
//  This test fails without try-catch handling in riscv/insns/ebreak.h.

#define SYS_EXIT_EXTENDED               0x20
#define ADP_Stopped_ApplicationExit     0x20026
#define CAUSE_BREAKPOINT                0x3

.option norvc

.section .ebreak, "ax"
ebreak_site:
  ebreak
  j fail

.section .text.start, "ax"
.globl _start
_start:
  la t0, trap_handler
  csrw mtvec, t0
  j ebreak_site

.section .text.handler, "ax"
trap_handler:
  csrr t0, mcause
  li t1, CAUSE_BREAKPOINT
  beq t0, t1, pass

  csrr t0, mepc
  addi t0, t0, 4
  csrw mepc, t0
  mret

pass:
  la a1, pass_args
  j exit_extended

fail:
  la a1, fail_args
  j exit_extended

exit_extended:
  li a0, SYS_EXIT_EXTENDED
  // RISC-V semihosting sequence
  slli zero, zero, 0x1f
  ebreak
  srai zero, zero, 7
1:
  j 1b

.section .rodata, "a"
.balign 8
#if __riscv_xlen == 64
pass_args:
  .dword ADP_Stopped_ApplicationExit, 0
fail_args:
  .dword ADP_Stopped_ApplicationExit, 1
#else
pass_args:
  .word ADP_Stopped_ApplicationExit, 0
fail_args:
  .word ADP_Stopped_ApplicationExit, 1
#endif