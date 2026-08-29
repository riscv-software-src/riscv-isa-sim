#ifndef MNRET_MNPP
#define MNRET_MNPP 0
#endif

#ifndef MNRET_MNPV
#define MNRET_MNPV 0
#endif

  .section .text.init
  .globl _start
_start:
#if __riscv_xlen == 64
  li t0, 1 << 59
  csrw menvcfg, t0
  csrw henvcfg, t0
#else
  li t0, 1 << 27
  csrw menvcfgh, t0
  csrw henvcfgh, t0
#endif

  li t0, 1 << 24
  csrw vsstatus, t0

#if __riscv_xlen == 64
  li t0, (1 << 42) | (1 << 24) | (1 << 17)
  csrw mstatus, t0
#else
  li t0, 1 << 10
  csrw mstatush, t0
  li t0, (1 << 24) | (1 << 17)
  csrw mstatus, t0
#endif

  li t0, (MNRET_MNPP << 11) | (MNRET_MNPV << 7)
  csrw 0x744, t0
  lla t0, mnret_target
  csrw 0x741, t0
  .word 0x70200073

  .section .mnret_target,"ax",@progbits
mnret_target:
  j mnret_target
