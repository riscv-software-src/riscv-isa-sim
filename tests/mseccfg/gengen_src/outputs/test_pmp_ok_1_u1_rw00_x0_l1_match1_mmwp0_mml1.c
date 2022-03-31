
/*
 * outputs/test_pmp_ok_1_u1_rw00_x0_l1_match1_mmwp0_mml1.c
 * Generated from gen_pmp_test.cc and test_pmp_ok_1.cc_skel.
 * 
 * This test program is expected to start executed from M mode.
 * That will be easier for us to deal with pmp exception for test.
 * 
 * Remarks:
 * - RW=01 not covered. U/M mode share will be tested separately
 * - RLB is always 0. CSR access control will be tested separately
 * 
 * @changed 2020-Mar-2 soberl
 *  For RWXL + MML, need to separate R and W combinations.
 *  Skip RW=01 (share mode) at generator driver side.
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

static const unsigned long expected_rw_fail = 1;
static unsigned actual_rw_fail = 0;

static const unsigned long expected_x_fail = 1;
static unsigned actual_x_fail = 0;
static void checkTestResult(void);

/*
 * override syscalls.c.
 *  currently simply skip to nexp instruction
 */
uintptr_t handle_trap(uintptr_t cause, uintptr_t epc, uintptr_t regs[32])
{
    if (epc >= TEST_MEM_START && epc < TEST_MEM_END) {
        asm volatile ("nop");
        actual_x_fail = 1;
        checkTestResult();
    } else if (cause == CAUSE_LOAD_ACCESS || cause == CAUSE_STORE_ACCESS) {
        reg_t addr;
        asm volatile ("csrr %0, mtval\n" : "=r"(addr));
//        printf("addr = 0x%x\n", addr);
        if (addr >= TEST_MEM_START && addr < TEST_MEM_END) {
            actual_rw_fail = 1;
            return epc + 4;
        }
        
        if (addr == FAKE_ADDRESS) {
            asm volatile ("nop");
            asm volatile ("nop");
            checkTestResult();
        }
    }
    
    printf("cause = %ld, epc = 0x%lx\n", cause, epc);
    tohost_exit(1337);
}


// switch (eret) to U mode and resume next PC
static void switch_to_U() {
    reg_t tmp;
    asm volatile (
            "li %0, %1\n"
            "\tcsrc mstatus, t0\n"
            "\tla %0, try_access_umode \n"
            "\tcsrw mepc, %0\n"
            "\tli sp, %2\n"
            "\tmret\n"
            : "=r"(tmp) : "n"(MSTATUS_MPP), "n"(U_MEM_END) : "memory");
}

static void switch_mode() {
#if 1
    switch_to_U();
#endif
}

__attribute ((section(".text_test_foo"), noinline))
static void target_foo() {
    asm volatile ("nop");
    
    actual_x_fail = 0;
}

/*
 * avoid to access actual_x_fail lies in M mode
 */
__attribute ((section(".text_test_foo"), noinline))
static void target_foo_umode() {
    asm volatile ("nop");
}

__attribute ((section(".data_test_arr"), aligned(8)))
static volatile unsigned char target_arr[100] = {
        1,2,3,4,5,6,7,8,
};

static int detect_pmp_granularity(){
    unsigned int granule;
    unsigned long int temp_reg;
    unsigned long int all_ones = 0xffffffffffffffffULL;

    asm volatile ("csrw pmpaddr0, %0 \n" :: "r"(all_ones) : "memory");
    asm volatile ("csrr %0, pmpaddr0 \n" : "=r"(temp_reg));
    asm volatile ("csrw pmpaddr0, %0 \n" :: "r"(0x0) : "memory");

    int g = 2;
    for(uintptr_t i = 1; i; i<<=1) {
        if((temp_reg & i) != 0)
            break;
        g++;
    }
    granule = 1UL << g;

    return granule;
}

static int mismatch_addr_offset(int granule_size){
    unsigned int addr_offset = 0;

    if (addr_offset == 0x0){
        return 0x0;
    }
    else {
        unsigned int mismatch_offset   = granule_size;
        while (mismatch_offset < addr_offset){
            mismatch_offset = mismatch_offset << 0x1;
        }
        return mismatch_offset;
    }
}

/*
 * On processor_t::reset():
 *  - set_csr(CSR_PMPADDR0, ~reg_t(0));
 *    set_csr(CSR_PMPCFG0, PMP_R | PMP_W | PMP_X | PMP_NAPOT);
 */
static void set_cfg() {
#if 1
    /*
     * set MSECCFG_RLB to avoid locked
     */
    unsigned rlb_value = MSECCFG_RLB;
    asm volatile ("csrs 0x747, %0 \n"::"r"(rlb_value));
#endif
    
    /*
     * Set pmp0cfg for M mode (M_MEM), and pmp1cfg for base of TOR.
     * Then use pmp2cfg for TEST_MEM. Both test code and data share PMP entrance.
     * Also use pmp3cfg for fixed U mode (U_MEM).
     * 
     * Here @pmp_addr_offset:int@ is to create an address mismatch
     * And @create_pmp_cfg:int@ is to create cfg mismatch.
     */

    unsigned int mismatch_offset = 0;

    if (mismatch_offset != 0x0){
        volatile int pmp_granularity = detect_pmp_granularity();
        mismatch_offset = mismatch_addr_offset(pmp_granularity);
    }

    asm volatile ("csrw pmpaddr3, %0 \n" :: "r"(U_MEM_END >> 2) : "memory");
    asm volatile ("csrw pmpaddr2, %0 \n" :: "r"(TEST_MEM_END >> 2) : "memory");
    asm volatile ("csrw pmpaddr1, %0 \n" :: "r"((TEST_MEM_START + mismatch_offset) >> 2) : "memory");
    
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
    
    // Only true for Spike
//    asm volatile ("csrr %0, pmpcfg0\n":"=r"(cfg0)); 
//    if (cfg0 != (PMP_R | PMP_W | PMP_X | PMP_NAPOT)) {
//        exit(cfg0);
//    }
    
    if (1) {    // need to set L bit for M mode code like trap_handling
#if M_MODE_RWX
        cfg0 |= PMP_L;
#else
        cfg1 |= ((PMP_L << 8) | (PMP_L << 16));
#endif
    }
    
    cfg0 |= (PMP_R | PMP_W | PMP_X | PMP_TOR) << 24;    // for U_MEM
#if 1
    cfg0 |= ( (0 ? PMP_R : 0)
            | (0 ? PMP_W : 0)
            | (0 ? PMP_X : 0) 
            | PMP_TOR | (1 ? PMP_L : 0)) << 16;
#endif   
    
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
    
    // set proc->state.mseccfg, for MML/MMWP
    const unsigned seccfg_bits = (1 ? MSECCFG_MML : 0) | (0 ? MSECCFG_MMWP : 0);
    if (seccfg_bits) {
        asm volatile ("csrs 0x747, %0 \n"::"r"(seccfg_bits));
    }
    
    // currently dummy since tlb flushed when set_csr on mseccfg
    asm volatile ("fence.i \n");
}

// from pmp_ok() side,W/R/X is similar
__attribute ((noinline))
static void try_access() {
#if TEST_RW
    target_arr[0] += 1;
    const unsigned long delta = 0x1020304005060708UL;
    *(long *)target_arr += delta;

    if (*(long *)target_arr != 0x0807060504030201UL + delta + 1) {
        actual_rw_fail = 1;
    }
#endif

#if TEST_FETCH
    actual_x_fail = 1;  // reset inside target_foo()
    target_foo();
#endif
}

// in case mml set, printf cannot be used in U mode
__attribute ((section(".text_umode")))
void try_access_umode() {
#if TEST_RW
    target_arr[0] += 1;
//    const unsigned long delta = 0x1020304005060708UL;
//    *(long *)target_arr += delta;

//    if (*(long *)target_arr != 0x0807060504030201UL + delta + 1) {
//        actual_rw_fail = 1;
//    }
#endif

#if TEST_FETCH
    target_foo_umode();
#endif
    
    /*
     * switch to M mode by invoking a write access fault for special address.
     */ 
    *(char *)(FAKE_ADDRESS) = 1;
}

static void checkTestResult() {
    int ret = 0;
    if (expected_rw_fail != actual_rw_fail) {
        ret += 1;
        printf("Read/write test fail, expected %d, actual %d.\n", expected_rw_fail, actual_rw_fail);
    }

    if (expected_x_fail != actual_x_fail) {
        ret += 2;
        printf("Fetch test fail, expected %d, actual %d.\n", expected_x_fail, actual_x_fail);
    }
    
    printf("Test done, exit %d.\n", ret);
    
    exit(ret); 
}

int main() {
    // assert in M mode
    set_cfg();

    switch_mode();  // in case swith to u mode, branch to try_access_umode directly

    try_access();

    checkTestResult();
    return 0; // assert 0
}

