#include <dlfcn.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <vector>

typedef uint64_t word_t;
typedef int64_t sword_t;

typedef word_t rtlreg_t;
typedef word_t vaddr_t;
typedef uint32_t paddr_t;

typedef struct CPU_STATE{
    rtlreg_t gpr[32];
    rtlreg_t pc;
    rtlreg_t csr[7];
}CPU_state;

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void (*ref_difftest_memcpy_from_dut)(paddr_t dest, void *src, uint64_t n) = NULL;
void (*ref_difftest_getregs)(void *c) = NULL;
void (*ref_difftest_setregs)(const void *c) = NULL;
void (*ref_difftest_exec)(uint64_t n) = NULL;
void (*ref_check_end)(void* indi) = NULL;

void (*dut_difftest_memcpy_from_dut)(paddr_t dest, void *src, uint64_t n) = NULL;
void (*dut_difftest_getregs)(void *c) = NULL;
void (*dut_difftest_setregs)(const void *c) = NULL;
void (*dut_difftest_exec)(uint64_t n) = NULL;
void (*dut_check_end)(void* indi) = NULL;

#define MAX_PROGRAM_SIZE 0x8000000
#define PC_START 0x80100000
uint8_t program[MAX_PROGRAM_SIZE];
int program_sz;
CPU_state state;

int check = 0;

void load_program(char* filename){
    memset(&program, 0, sizeof(program));
    if(!filename){
        printf("Use the default build-in program\n");
        ((uint32_t*)program)[0] = 0x00000297;  // lui t0,0x80000
        ((uint32_t*)program)[1] = 0x0002b823;  // sd  zero,0(t0)
        ((uint32_t*)program)[2] = 0x0102b503;  // ld  a0,16(t0)
        ((uint32_t*)program)[3] = 0x0000006b;  // nemu_trap
        program_sz = 16;
        return;
    }
    printf("Load %s...\n", filename);
    FILE* fp = fopen(filename, "rb");
    assert(fp);

    fseek(fp, 0, SEEK_END);
    program_sz = ftell(fp);
    assert(program_sz < MAX_PROGRAM_SIZE);

    fseek(fp, 0, SEEK_SET);
    int ret = fread(program, program_sz, 1, fp);
    assert(ret == 1);
    printf("load program size: %x\n", program_sz);
    return;
}

void init_difftest(char *ref_so_file, char* dut_so_file){
    memset(state.gpr, 0, sizeof(state.gpr));

    state.pc = PC_START;

    assert(ref_so_file != NULL);

    void* handle;
    handle = dlopen(ref_so_file,  RTLD_LAZY | RTLD_DEEPBIND);
    assert(handle);

    ref_difftest_memcpy_from_dut = (void (*)(paddr_t dest, void *src, size_t n))dlsym(handle, "_Z24difftest_memcpy_from_dutmPvm");
    assert(ref_difftest_memcpy_from_dut);

    ref_difftest_getregs = (void (*)(void *))dlsym(handle, "_Z16difftest_getregsPv");
    assert(ref_difftest_getregs);

    ref_difftest_setregs = (void (*)(const void *))dlsym(handle, "_Z16difftest_setregsPv");
    assert(ref_difftest_setregs);

    ref_difftest_exec = (void (*)(uint64_t))dlsym(handle, "_Z13difftest_execv");
    assert(ref_difftest_exec);

    void (*ref_difftest_init)(void) = (void (*)(void))dlsym(handle, "_Z13difftest_initv");
    assert(ref_difftest_init);

    ref_difftest_init();

    ref_difftest_memcpy_from_dut(PC_START, (void*)program, program_sz);
    ref_difftest_setregs(&state);


    handle = dlopen(dut_so_file,  RTLD_LAZY | RTLD_DEEPBIND);
    assert(handle);

    dut_difftest_memcpy_from_dut = (void (*)(paddr_t dest, void *src, size_t n))dlsym(handle, "isa_difftest_memcpy_from_dut");
    assert(dut_difftest_memcpy_from_dut);

    dut_difftest_getregs = (void (*)(void *))dlsym(handle, "isa_difftest_getregs");
    assert(dut_difftest_getregs);

    dut_difftest_setregs = (void (*)(const void *))dlsym(handle, "isa_difftest_setregs");
    assert(dut_difftest_setregs);

    dut_difftest_exec = (void (*)(uint64_t))dlsym(handle, "cpu_exec");
    assert(dut_difftest_exec);

    dut_check_end = (void (*)(void *))dlsym(handle, "is_nemu_trap");
    assert(dut_check_end);

    void (*dut_difftest_init)(void) = (void (*)(void))dlsym(handle, "isa_difftest_init");
    assert(dut_difftest_init);

    dut_difftest_init();

    dut_difftest_memcpy_from_dut(PC_START, (void*)program, program_sz);
    dut_difftest_setregs(&state);

}
void print_info(CPU_state* ref_r, CPU_state* dut_r){

   for(int i = 0; i < 32; i++){
            printf("%-4s    %016lx  | %016lx  \n", regs[i], ref_r->gpr[i], dut_r->gpr[i]);
        }
    printf("pc      %16lx    %16lx\n", ref_r->pc, dut_r->pc);

}

bool check_gregs(CPU_state* ref_r, CPU_state* dut_r){
    if(!check) return 1;
    for(int i = 0; i < 32; i++){
        if(ref_r->gpr[i] != dut_r->gpr[i]){
            printf("reg: %s\n", regs[i]);
            return false;
        }
    }
    if(ref_r->pc != dut_r->pc) return false;
    return true;
}

int is_diff = 0;
void difftest_step(uint32_t n){
    CPU_state ref_r;
    CPU_state dut_r;
    while(n --){
        ref_difftest_exec(1);
        ref_difftest_getregs(&ref_r);

        dut_difftest_exec(1);
        dut_difftest_getregs(&dut_r);
        if(!check_gregs(&ref_r, &dut_r)){
            print_info(&ref_r, &dut_r);
        }
    }
}

/*cpu tests
add-longlong-riscv64-nemu   fact-riscv64-nemu           matrix-mul-riscv64-nemu     prime-riscv64-nemu      string-riscv64-nemu
fib-riscv64-nemu            max-riscv64-nemu            sub-longlong-riscv64-nemu
add-riscv64-nemu            quick-sort-riscv64-nemu     goldbach-riscv64-nemu       min3-riscv64-nemu       sum-riscv64-nemu
bit-riscv64-nemu            recursion-riscv64-nemu      hello-str-riscv64-nemu      mov-c-riscv64-nemu      switch-riscv64-nemu
bubble-sort-riscv64-nemu    if-else-riscv64-nemu        select-sort-riscv64-nemu    movsx-riscv64-nemu      to-lower-case-riscv64-nemu
div-riscv64-nemu            leap-year-riscv64-nemu      shift-riscv64-nemu          mul-longlong-riscv64-nemu
unalign-riscv64-nemu        dummy-riscv64-nemu          load-store-riscv64-nemu     shuixianhua-riscv64-nemu
pascal-riscv64-nemu         wanshu-riscv64-nemu
*/

int main(){
    char filename[] = "../../../riscv64-processor/am-kernels/benchmarks/microbench/build/microbench-riscv64-nemu.bin";
    load_program(filename);

    char ref_so_path[] = "build/difftest.so";
    char dut_so_path[] = "../../../riscv64-processor/nemu/build/riscv64-nemu-interpreter-so";
    init_difftest(ref_so_path, dut_so_path);
    printf("after finishlization\n");

    uint32_t is_end = 0;
    while(!is_end && !is_diff){
        difftest_step(1);
        dut_check_end(&is_end);
    }
}
