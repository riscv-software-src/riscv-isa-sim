// ctxsw_test.c
#include <stdint.h>
#include <stdio.h>

#define CSR_UCTX_OUT   0x8c0
#define CSR_UCTX_IN    0x8c1
#define CSR_UCTX_COUNT 0x8c2
#define CSR_UCTX_TOTAL 0x8c3
#define CSR_UCTX_LAST  0x8c4
#define CSR_UCTX_MIN   0x8c5
#define CSR_UCTX_MAX   0x8c6
#define CSR_UCTX_AVG   0x8c7
#define CSR_UCTX_CTRL  0x8c8

// CSR helpers require the CSR operand to be an immediate. Keep them as macros so
// the assembler sees the numeric literal.
#define CSRW_IMM(csr_imm, val) \
  asm volatile("csrw %0, %1" :: "i"(csr_imm), "rK"(val) : "memory")

#define CSRR_IMM(csr_imm) \
  ({ uint64_t __v; asm volatile("csrr %0, %1" : "=r"(__v) : "i"(csr_imm) : "memory"); __v; })

static void spin_cycles(volatile uint64_t n) {
  while (n--) asm volatile ("" ::: "memory");
}

int main(void) {
  // Reset counters and enable print-on-read (bit0=print, bit1=reset)
  CSRW_IMM(CSR_UCTX_CTRL, 0x3);

  // First context switch sample
  CSRW_IMM(CSR_UCTX_OUT, 0);
  spin_cycles(1000);
  CSRW_IMM(CSR_UCTX_IN, 0);

  // Second sample with a different duration
  CSRW_IMM(CSR_UCTX_OUT, 0);
  spin_cycles(5000);
  CSRW_IMM(CSR_UCTX_IN, 0);

  uint64_t count = CSRR_IMM(CSR_UCTX_COUNT);
  uint64_t total = CSRR_IMM(CSR_UCTX_TOTAL);
  uint64_t last  = CSRR_IMM(CSR_UCTX_LAST);
  uint64_t min   = CSRR_IMM(CSR_UCTX_MIN);
  uint64_t max   = CSRR_IMM(CSR_UCTX_MAX);
  uint64_t avg   = CSRR_IMM(CSR_UCTX_AVG);

  printf("ctx count=%llu total=%llu last=%llu min=%llu max=%llu avg=%llu\n",
         (unsigned long long)count,
         (unsigned long long)total,
         (unsigned long long)last,
         (unsigned long long)min,
         (unsigned long long)max,
         (unsigned long long)avg);
  return 0;
}

/*

riscv64-unknown-elf-gcc -O0 -march=rv64gc -mabi=lp64 -static ctxsw_test.c -o ctxsw_test
spike pk ./ctxsw_test

*/