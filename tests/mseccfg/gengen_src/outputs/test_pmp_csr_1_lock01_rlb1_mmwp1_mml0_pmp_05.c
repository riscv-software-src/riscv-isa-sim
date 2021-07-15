
/*
 * outputs/test_pmp_csr_1_lock01_rlb1_mmwp1_mml0_pmp_05.c
 * Generated from gen_pmp_test.cc and test_pmp_csr_1.cc_skel.
 * 
 * This test program is verify the pmp CSR access when seccfg introduced.
 * It's expected to executed from M mode.
 * 
 * Remarks:
 * - CSR protection for non-M mode access is assumed and not coverred.
 * - The access on invalid CSR index like pmpcfg1 for rv64 is not coverred.
 * - Executed on RV64 only.
 */

/*
 * Macros from encoding.h
 */
#define MSTATUS_MPP         0x00001800

#define PMP_R     0x01
#define PMP_W     0x02
#define PMP_X     0x04
#define PMP_A     0x18
#define PMP_L     0x80
#define PMP_SHIFT 2

#if ((PMP_R | PMP_W | PMP_X) != 0x7)
#error unexpected
#endif

#define PMP_OFF   0x0
#define PMP_TOR   0x08
#define PMP_NA4   0x10
#define PMP_NAPOT 0x18

#define MSECCFG_MML  0x1
#define MSECCFG_MMWP 0x2
#define MSECCFG_RLB  0x4

#define TEST_RW 1
#define TEST_FETCH 1
/*
 * Whether rwx share single cfg for M mode
 * When @set_sec_mml@ set, it must be 0, otherwise unexpected exception
 */
#define M_MODE_RWX 0

#define CAUSE_LOAD_ACCESS 0x5
#define CAUSE_STORE_ACCESS 0x7

typedef unsigned long reg_t;
typedef unsigned long uintptr_t;

/*
 * functions from syscalls.c
 */
#if PRINTF_SUPPORTED
int printf(const char* fmt, ...);
#else
#define printf(...)
#endif

void __attribute__((noreturn)) tohost_exit(uintptr_t code);
void exit(int code);

/*
 * local status
 */
#define TEST_MEM_START 0x200000
#define TEST_MEM_END 0x240000
#define U_MEM_END (TEST_MEM_END + 0x10000)
#define FAKE_ADDRESS 0x10000000

static const unsigned long expected_seccfg_fail = 0;
static unsigned actual_seccfg_fail = 0;

static const unsigned long expected_pmpaddr_fail = 0;
static unsigned actual_pmpaddr_fail = 0;

static const unsigned long expected_pmpcfg_fail = 1;
static unsigned actual_pmpcfg_fail = 0;

static void checkTestResult(void);

/*
 * override syscalls.c.
 *  currently simply skip to nexp instruction
 */
uintptr_t handle_trap(uintptr_t cause, uintptr_t epc, uintptr_t regs[32])
{ 
    printf("cause = %ld, epc = 0x%lx\n", cause, epc);
    tohost_exit(1337);
}


__attribute ((section(".text_test_foo"), noinline))
void target_foo() {
    asm volatile ("nop");
}

__attribute ((section(".data_test_arr"), aligned(8)))
volatile unsigned char target_arr[100] = {
        1,2,3,4,5,6,7,8,
};

__attribute ((section(".text_umode"), noinline))
void target_foo_U() {
    asm volatile ("nop");
}

__attribute ((section(".data_umode"), aligned(8)))
volatile unsigned char target_arr_U[100] = {
        1,2,3,4,5,6,7,8,
};

/*
 * On processor_t::reset():
 *  - set_csr(CSR_PMPADDR0, ~reg_t(0));
 *    set_csr(CSR_PMPCFG0, PMP_R | PMP_W | PMP_X | PMP_NAPOT);
 */
static void set_cfg() {
#if 1 // @set_rlb_at_start:int@
    /*
     * set MSECCFG_RLB to avoid locked at start
     */
    asm volatile ("csrs 0x747, %0 \n"::"r"(MSECCFG_RLB));
    asm volatile ("nop");
#endif
    
//------------------------Set current status before the test target (CSR access)
    /*
     * Set pmp0cfg for M mode (M_MEM), and pmp1cfg for base of TOR.
     * Then use pmp2cfg for TEST_MEM. Both test code and data share PMP entrance.
     * Also use pmp3cfg for fixed U mode (U_MEM).
     */
    asm volatile ("csrw pmpaddr3, %0 \n" :: "r"(U_MEM_END >> 2) : "memory");
    asm volatile ("csrw pmpaddr2, %0 \n" :: "r"(TEST_MEM_END >> 2) : "memory");
    asm volatile ("csrw pmpaddr1, %0 \n" :: "r"(TEST_MEM_START >> 2) : "memory");
    
#if M_MODE_RWX
    asm volatile ("csrw pmpaddr0, %0 \n" :: "r"((TEST_MEM_START >> 3) - 1) : "memory");
    reg_t cfg0 = (PMP_R | PMP_W | PMP_X | PMP_NAPOT);
#else
    asm volatile ("csrw pmpaddr6, %0 \n" :: "r"(TEST_MEM_START >> 2) : "memory"); // for data
    asm volatile ("csrw pmpaddr5, %0 \n" :: "r"(0x110000 >> 2) : "memory");       // for code
    asm volatile ("csrw pmpaddr4, %0 \n" :: "r"(0x100000 >> 2) : "memory");       // addr start
    reg_t cfg0 = PMP_OFF;
    reg_t cfg1 = PMP_OFF | ((PMP_R | PMP_W | PMP_TOR) << 16) | ((PMP_X | PMP_TOR) << 8);
#endif

    if (0) {    // need to set L bit for M mode code access
#if M_MODE_RWX
        cfg0 |= PMP_L;
#else
        cfg1 |= ((PMP_L << 8) | (PMP_L << 16));
#endif
    }
    
    reg_t sub_cfg = PMP_R | PMP_W | PMP_X | PMP_TOR | ((1 || 0) ? PMP_L : 0);
    cfg0 |= sub_cfg << 24;    // for U_MEM
    cfg0 |= sub_cfg << 16;    // for TEST_MEM
    
#if !M_MODE_RWX
#if __riscv_xlen == 64
    cfg0 |= (cfg1 << 32);
#else
    asm volatile ("csrw pmpcfg1, %0 \n"
                :
                : "r"(cfg1)
                : "memory");
#endif // __riscv_xlen == 64
#endif // !M_MODE_RWX
    
    asm volatile ("csrw pmpcfg0, %0 \n"
                :
                : "r"(cfg0)
                : "memory");

    if (1 != 0) {
        reg_t lock_bits = (PMP_L << 16) | ((reg_t)PMP_L << 24); // avoid use (default) int type
        if (0) {
            asm volatile ("csrs pmpcfg0, %0 \n"
                            :
                            : "r"(lock_bits)
                            : "memory");
        } else {
            asm volatile ("csrc pmpcfg0, %0 \n"
                            :
                            : "r"(lock_bits)
                            : "memory");
        }
    }
    
    // set proc->state.mseccfg
    const unsigned seccfg_bits = (1 ? MSECCFG_RLB : 0) 
            | (0 ? MSECCFG_MML : 0) 
            | (1 ? MSECCFG_MMWP : 0);
    asm volatile ("csrw 0x747, %0 \n"::"r"(seccfg_bits));
    
//------------------------Test target
    asm volatile ("nop");
    /*
     * Need to separate pmp and seccfg access since pmplock_recorded status may be 
     * updated again when accessing pmpcfg.
     */
    reg_t wval = 0, rval;
#if 1
    asm volatile ("csrr %0, pmpaddr3 \n"
            : "=r"(rval));
    // give a valid value for both NAPOT and TOR
    if (3 == 0) {
        wval = ((rval + 1) << 1) - 1;   // NAPOT mask
    } else {
        wval = (rval << 1) + 0;   
    }
    asm volatile ("csrw pmpaddr3, %1 \n"
                "\tcsrr %0, pmpaddr3 \n"
            : "=r"(rval)
            : "r"(wval)
              : "memory");
    if (wval != rval) {
        printf("pmpaddr3 expects %lx vs %lx\n", wval, rval);
        actual_pmpaddr_fail = 1;
    }
    
    wval = (0 == 0 ? cfg0 : 0) 
            ^ (5 << (3 * 8));
    asm volatile ("csrw pmpcfg0, %1 \n"
                "\tcsrr %0, pmpcfg0 \n"
            : "=r"(rval)
            : "r"(wval)
              : "memory");
    if (wval != rval) {
        printf("pmpcfg expects %lx vs %lx\n", wval, rval);
        actual_pmpcfg_fail = 1;
    }
#else
    /*
     * need to set PMP_L for cfg0 otherwise next PC will illegal
     * This is a little coverage hole for non-PMP_L + mml, which should be
     * a restricted use case and can be accepted anyway.
     */ 
    if (1) {    
#if M_MODE_RWX
        asm volatile ("csrs pmpcfg0, %0 \n"::"r"(PMP_L));
#else
#if __riscv_xlen == 64
        asm volatile ("csrs pmpcfg0, %0 \n"::"r"(((reg_t)PMP_L << 40) | ((reg_t)PMP_L << 48)));
#else
        asm volatile ("csrs pmpcfg1, %0 \n"::"r"((PMP_L << 8) | (PMP_L << 16)));
#endif // __riscv_xlen == 64
#endif // M_MODE_RWX
    }
    
    wval = (1 ? MSECCFG_RLB : 0) 
            | (1 ? MSECCFG_MML : 0) 
            | (1 ? MSECCFG_MMWP : 0);
    asm volatile ("csrw 0x747, %1 \n"
                "\tcsrr %0, 0x747 \n"
            : "=r"(rval)
            : "r"(wval)
              : "memory");
    reg_t expected_val = wval & (MSECCFG_RLB | MSECCFG_MML | MSECCFG_MMWP);
    /*
     * pre_sec_mml means cfg0 locked
     * pmp_lock means cfg2/3 locked
     * sec_mml is the test coverage hole just mentioned
     */
    if ((0 || 0 || 1) 
            && 1 == 0) {
        expected_val &= ~MSECCFG_RLB;
    } 
    if (0) {
        expected_val |= MSECCFG_MML;
    }
    if (1) {
        expected_val |= MSECCFG_MMWP;
    }
    
    if (expected_val != rval) actual_seccfg_fail = 1;
#endif
}

static void checkTestResult() {
    int ret = 0;
    if (expected_seccfg_fail != actual_seccfg_fail) {
        ret += 1;
        printf("Access seccfg fail, expected %d, actual %d.\n", expected_seccfg_fail, actual_seccfg_fail);
    }

    if (expected_pmpaddr_fail != actual_pmpaddr_fail) {
        ret += 2;
        printf("Access pmpaddr fail, expected %d, actual %d.\n", expected_pmpaddr_fail, actual_pmpaddr_fail);
    }
    
    if (expected_pmpcfg_fail != actual_pmpcfg_fail) {
        ret += 4;
        printf("Access pmpcfg fail, expected %d, actual %d.\n", expected_pmpcfg_fail, actual_pmpcfg_fail);
    }
    
    printf("Test done, exit %d.\n", ret);
    
    exit(ret); 
}

int main() {
    // assert in M mode
    set_cfg();

    checkTestResult();
    return 0; // assert 0
}

