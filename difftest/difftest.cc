#include "sim.h"
static std::vector<std::pair<reg_t, abstract_device_t*>> difftest_plugin_devices;
static std::vector<std::string> difftest_htif_args;
static std::vector<std::pair<reg_t, mem_t*>> difftest_mem(1, std::make_pair(reg_t(DRAM_BASE), new mem_t(reg_t(2048) << 20)));
static std::vector<int> difftest_hartids;
static debug_module_config_t difftest_dm_config = {
    .progbufsize = 2,
    .max_bus_master_bits = 0,
    .require_authentication = false,
    .abstract_rti = 0,
    .support_hasel = true,
    .support_abstract_csr_access = true,
    .support_haltgroups = true,
    .support_impebreak = true
};

struct diff_context_t{
    reg_t gpr[32];
    reg_t pc;
};

static sim_t* s;

void sim_t::diff_step_once(){
    step(1);
}

context_t* diff_host;
context_t diff_target;

void sim_t::diff_get_regs(void* diff_context){
    struct diff_context_t* ctx = (struct diff_context_t*)diff_context;
    processor_t *p = get_core("0");
    for(int i = 0; i < NXPR; i++){
        ctx->gpr[i] = p->get_state()->XPR[i];
    }
    ctx->pc = p->get_state()->pc;
}

void sim_t::diff_set_regs(void* diff_context){
    struct diff_context_t* ctx = (struct diff_context_t*)diff_context;
    processor_t *p = get_core("0");
    state_t *state = p->get_state();
    for(int i = 0; i < NXPR; i++){
        state->XPR.write(i, ctx->gpr[i]);
    }
    state->pc = ctx->pc;
}

void sim_t::diff_memcpy(reg_t dest, void* src, size_t n){
    processor_t *p = get_core("0");
    mmu_t* mmu = p->get_mmu();
    for(size_t i = 0; i < n; i++){
        mmu->store_uint8(dest+i, *((uint8_t*)src+i));
    }
}

void sim_t::diff_idle(){
    idle();
}

void difftest_memcpy_from_dut(reg_t dest, void *src, size_t n){
    s->diff_memcpy(dest, src, n);
}

void difftest_getregs(void* diff_context){
    s->diff_get_regs(diff_context);
}

void difftest_setregs(void* diff_context){
    s->diff_set_regs(diff_context);
}

void difftest_exec(){
    s->diff_step_once();
}

void difftest_init(){
    difftest_htif_args.push_back("./build/hello-spike.elf");
    s = new sim_t(DEFAULT_ISA, DEFAULT_PRIV, DEFAULT_VARCH, 1, false, false,
        0, 0, NULL, reg_t(-1), difftest_mem, difftest_plugin_devices, difftest_htif_args,
        std::move(difftest_hartids), difftest_dm_config, nullptr, true, NULL);
}
