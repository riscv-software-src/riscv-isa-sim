#include "sim.h"
#include "include/common.h"
#include "include/difftest-def.h"
#include "include/dummy_debug.h"

static std::vector<std::pair<reg_t, abstract_device_t*>> difftest_plugin_devices(
 1, std::make_pair(reg_t(DM_BASE_ADDR), new dummy_debug_t));
static std::vector<std::string> difftest_htif_args;
static std::vector<std::pair<reg_t, mem_t*>> difftest_mem(
    1, std::make_pair(reg_t(DRAM_BASE), new mem_t(CONFIG_MSIZE)));
static std::vector<int> difftest_hartids;
static debug_module_config_t difftest_dm_config = {
  .progbufsize = 2,
  .max_bus_master_bits = 0,
  .require_authentication = false,
  .abstract_rti = 0,
  .support_hasel = true,
  .support_abstract_csr_access = true,
  .support_haltgroups = true,
  .support_impebreak = false
};
static csr_t_p mscratch = nullptr;

struct diff_context_t {
  word_t gpr[32];
  word_t fpr[32];
  word_t pc;
  word_t mstatus;
  word_t mcause;
  word_t mepc;
  word_t sstatus;
  word_t scause;
  word_t sepc;
  word_t satp;
  word_t mip;
  word_t mie;
  word_t mscratch;
  word_t sscratch;
  word_t mideleg;
  word_t medeleg;
  word_t mtval;
  word_t stval;
  word_t mtvec;
  word_t stvec;
  word_t priv;
  word_t debugMode;
  word_t dcsr;
  word_t dpc;
  word_t dscratch0;
  word_t dscratch1;
};

struct diff_gpr_pc_p {
  volatile const reg_t *volatile gpr;
  volatile const reg_t *pc;
};

struct diff_gpr_pc_p diff_gpr_pc = {};

static sim_t *s = NULL;
static processor_t *p = NULL;
static state_t *state = NULL;

void sim_t::diff_init(int port) {
  p = get_core("0");
  state = p->get_state();
  diff_gpr_pc.gpr = state->XPR.get_addr();
  diff_gpr_pc.pc = &(state->pc);
  mscratch = state->csrmap.find(CSR_MSCRATCH)->second;
}

void sim_t::diff_step(uint64_t n) {
  step(n);
}

void sim_t::diff_get_regs(void* diff_context) {
  struct diff_context_t *ctx = (struct diff_context_t *)diff_context;
  for (int i = 0; i < NXPR; i++) {
    ctx->gpr[i] = state->XPR[i];
  }
  for (int i = 0; i < NFPR; i++) {
    ctx->fpr[i] = f128_to_ui64_r_minMag(state->FPR[i], true);
  }
  ctx->pc = state->pc;
  ctx->mstatus = state->mstatus->read();
  ctx->mcause = state->mcause->read();
  ctx->mepc = state->mepc->read();
  ctx->sstatus = state->sstatus->read();
  ctx->scause = state->scause->read();
  ctx->sepc = state->sepc->read();
  ctx->satp = state->satp->read();
  ctx->mip = state->mip->read();
  ctx->mie = state->mie->read();
  ctx->mscratch = state->csrmap[CSR_MSCRATCH]->read();
  ctx->sscratch = state->csrmap[CSR_SSCRATCH]->read();
  ctx->mideleg = state->mideleg->read();
  ctx->medeleg = state->medeleg->read();
  ctx->mtval = state->mtval->read();
  ctx->stval = state->stval->read();
  ctx->mtvec = state->mtvec->read();
  ctx->stvec = state->stvec->read();
  ctx->priv = state->prv;
  ctx->debugMode = state->debug_mode;
  ctx->dcsr = state->dcsr->read();
  ctx->dpc = state->dpc->read();
  ctx->dscratch0 = state->csrmap[CSR_DSCRATCH0]->read();
  ctx->dscratch1 = state->csrmap[CSR_DSCRATCH1]->read();
}

void sim_t::diff_set_regs(void* diff_context) {
  struct diff_context_t* ctx = (struct diff_context_t*)diff_context;
  for (int i = 0; i < NXPR; i++) {
    state->XPR.write(i, (sword_t)ctx->gpr[i]);
  }
  for (int i = 0; i < NFPR; i++) {
    state->FPR.write(i, ui64_to_f128(ctx->fpr[i]));
  }
  state->pc = ctx->pc;
  state->mstatus->write(ctx->mstatus);
  state->mcause->write(ctx->mcause);
  state->mepc->write(ctx->mepc);
  state->sstatus->write(ctx->sstatus);
  state->scause->write(ctx->scause);
  state->sepc->write(ctx->sepc);
  state->satp->write(ctx->satp);
  state->mip->write(ctx->mip);
  state->mie->write(ctx->mie);
  state->csrmap[CSR_MSCRATCH]->write(ctx->mscratch);
  state->csrmap[CSR_SSCRATCH]->write(ctx->sscratch);
  state->mideleg->write(ctx->mideleg);
  state->medeleg->write(ctx->medeleg);
  state->mtval->write(ctx->mtval);
  state->stval->write(ctx->stval);
  state->mtvec->write(ctx->mtvec);
  state->stvec->write(ctx->stvec);
  state->prv = ctx->priv;
  state->debug_mode = ctx->debugMode;
  state->dcsr->write(ctx->dcsr);
  state->dpc->write(ctx->dpc);
  state->csrmap[CSR_DSCRATCH0]->write(ctx->dscratch0);
  state->csrmap[CSR_DSCRATCH1]->write(ctx->dscratch1);
}

void sim_t::diff_memcpy(reg_t dest, void* src, size_t n) {
  mmu_t* mmu = p->get_mmu();
  for (size_t i = 0; i < n; i++) {
    mmu->store_uint8(dest+i, *((uint8_t*)src+i));
  }
}

void sim_t::diff_debugmode(void){
  // Debug Intr causes entry to debug mode
  p->halt_request = p->HR_REGULAR;
  procs[current_proc]->step(0);
  p->halt_request = p->HR_NONE;
}

abstract_device_t * sim_t::get_debug_module(void){
  return s->plugin_devices.front().second;
}


extern "C" {

void difftest_memcpy(paddr_t addr, void *buf, size_t n, bool direction) {
  if (direction == DIFFTEST_TO_REF) {
    s->diff_memcpy(addr, buf, n);
  } else {
    assert(0);
  }
}

void difftest_regcpy(void* dut, bool direction) {
  if (direction == DIFFTEST_TO_REF) {
    s->diff_set_regs(dut);
  } else {
    s->diff_get_regs(dut);
  }
}

void difftest_exec(uint64_t n) {
  s->diff_step(n);
}

void difftest_init(int port) {
  difftest_htif_args.push_back("");
  s = new sim_t(
    // const char* isa
    "RV64IMAFDC_zba_zbb_zbc_zbs_zbkb_zbkc_zbkx_zknd_zkne_zknh_zksed_zksh_svinval",
    // const char* priv
    DEFAULT_PRIV,
    // const char* varch
    DEFAULT_VARCH,
    // size_t _nprocs
    1,
    // bool halted, bool real_time_clint
    false, false,
    // reg_t initrd_start, reg_t initrd_end, const char* bootargs
    0, 0, nullptr,
    // reg_t start_pc, std::vector<std::pair<reg_t, mem_t*>> mems
    reg_t(-1), difftest_mem,
    // std::vector<std::pair<reg_t, abstract_device_t*>> plugin_devices
    difftest_plugin_devices,
    // const std::vector<std::string>& args
    difftest_htif_args,
    // const std::vector<int> hartids
    std::move(difftest_hartids),
    // const debug_module_config_t &dm_config
    difftest_dm_config,
    // const char *log_path
    nullptr,
    //bool dtb_enabled, const char *dtb_file, FILE *cmd_file
    false, nullptr, nullptr);
  s->diff_init(port);
}

void difftest_raise_intr(uint64_t NO) {
  if (NO & 0xc) {
    printf("Debug Intr!!!\n");
    s->diff_debugmode();  // Debug Intr
  } else {
      printf("Other Intr!!!\n");
    state->mip->write(state->mip->read() | 0xa00UL);
    difftest_exec(1);
    state->mip->write(state->mip->read() & (~0xa00UL));
  }
}

void isa_reg_display() {
  printf("TODO isa_reg_display in Spike\n");
}

int difftest_store_commit(uint64_t *addr, uint64_t *data, uint8_t *mask) {
  return 0;
}

uint64_t difftest_guided_exec(void *) {
  difftest_exec(1);
  return 0;
}

void debug_mem_sync(reg_t addr, void* buf, size_t n) {
  // addr is absolute physical addr
  // now we are not using this right now because I don't know how spike does if from device
  dummy_debug_t* dummy_debug = (dummy_debug_t *) s->get_debug_module();
  // start addr is virtual addr used by simulator
  int offset = (addr - DM_BASE_ADDR) / sizeof(uint8_t);
  uint8_t* start_addr = dummy_debug->dummy_debug_mem + offset;
  memcpy(start_addr, buf, n); // TODO copy to device not addr
}

}
