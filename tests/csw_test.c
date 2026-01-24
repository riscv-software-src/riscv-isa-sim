#include <stdint.h>

/* Custom CSRs */
#define CSR_CSW_COUNT      0x7C0
#define CSR_CSW_LASTCAUSE  0x7C1
#define CSR_CSW_CYCLES      0x7C2
#define CSR_CSW_LAST_TID    0x7C3

#define STR1(x) #x
#define STR(x) STR1(x)

/* Correct CSR read macro */
#define read_csr(csr) ({                     \
    uint64_t __v;                            \
    asm volatile ("csrr %0, " #csr           \
                  : "=r"(__v));              \
    __v;                                     \
})

/*
 * Debug-visible memory log
 * Index meaning:
 * 0 = count before
 * 1 = count after
 * 2 = last cause
 * 3 = cycles accumulated
 * 4 = last task / context id
 */
volatile uint64_t csw_log[5];

int main(void)
{
    /* Snapshot before context switch */
    csw_log[0] = read_csr(0x7C0);   // CSW_COUNT
    csw_log[3] = read_csr(0x7C2);   // CSW_CYCLES

    /* Force a context switch */
    asm volatile ("ecall");

    /* Snapshot after context switch */
    csw_log[1] = read_csr(0x7C0);   // CSW_COUNT
    csw_log[2] = read_csr(0x7C1);   // CSW_LASTCAUSE
    csw_log[3] = read_csr(0x7C2);   // CSW_CYCLES
    csw_log[4] = read_csr(0x7C3);   // CSW_LAST_TID

    while (1);
}


/*
riscv64-unknown-elf-gcc \
  -march=rv64imac \
  -mabi=lp64 \
  -nostdlib \
  -nostartfiles \
  -fno-builtin \
  -T linker.ld \
  start.S csw_test.c \
  -o csw_test.elf


~/spike-sim-fyp/build/spike -d --extension=cswprof csw_test.elf

*/