#include "sim.h"
#include "include/common.h"
#include "include/difftest-def.h"

static std::vector<std::pair<reg_t, abstract_device_t*>> difftest_plugin_devices;
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
  .support_impebreak = true
};
static csr_t_p mscratch = nullptr;

struct diff_context_t {
  word_t gpr[32];
  word_t pc;
  word_t mstatus;
  word_t mepc;
  word_t sepc;
  word_t mtvec;
  word_t stvec;
  word_t mcause;
  word_t scause;
  word_t mie;
  word_t mscratch;
  word_t priv;
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
  ctx->pc = state->pc;
  ctx->mstatus = state->mstatus->read();
  ctx->mepc = state->mepc->read();
  ctx->sepc = state->sepc->read();
  ctx->mtvec = state->mtvec->read();
  ctx->stvec = state->stvec->read();
  ctx->mcause = state->mcause->read();
  ctx->scause = state->scause->read();
  ctx->mie = state->mie->read();
  ctx->mscratch = mscratch->read();
  ctx->priv = state->prv;
}

void sim_t::diff_set_regs(void* diff_context) {
  struct diff_context_t* ctx = (struct diff_context_t*)diff_context;
  for (int i = 0; i < NXPR; i++) {
    state->XPR.write(i, (sword_t)ctx->gpr[i]);
  }
  if (ctx->pc)      state->pc = ctx->pc;
  if (ctx->mstatus) state->mstatus->write(ctx->mstatus);
  if (ctx->mepc)    state->mepc->write(ctx->mepc);
  if (ctx->sepc)    state->sepc->write(ctx->sepc);
  if (ctx->mtvec)   state->mtvec->write(ctx->mtvec);
  if (ctx->stvec)   state->stvec->write(ctx->stvec);
  state->mcause->write(ctx->mcause);
  state->scause->write(ctx->scause);
  state->mie->write(ctx->mie);
  mscratch->write(ctx->mscratch);
  state->prv = ctx->priv;
}

void sim_t::diff_memcpy(reg_t dest, void* src, size_t n) {
  mmu_t* mmu = p->get_mmu();
  for (size_t i = 0; i < n; i++) {
    mmu->store_uint8(dest+i, *((uint8_t*)src+i));
  }
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
  s = new sim_t(DEFAULT_ISA, DEFAULT_PRIV, DEFAULT_VARCH, 1, false, false,
      0, 0, nullptr, reg_t(-1), difftest_mem, difftest_plugin_devices, difftest_htif_args,
      std::move(difftest_hartids), difftest_dm_config, nullptr, false, nullptr, nullptr);
  s->diff_init(port);
}

void difftest_raise_intr(uint64_t NO) {
  assert(0);
}

}
