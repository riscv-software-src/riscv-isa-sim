#include "decode_macros.h"
#include "difftest.h"
#include "disasm.h"
#include "softfloat.h"

// Some static utilities and configurations
static debug_module_config_t difftest_dm_config = {
  .progbufsize = 2,
  .max_sba_data_width = 0,
  .require_authentication = false,
  .abstract_rti = 0,
  .support_hasel = true,
  .support_abstract_csr_access = true,
  .support_abstract_fpr_access = true,
  .support_haltgroups = true,
  .support_impebreak = false
};
extern std::vector<std::pair<reg_t, mem_t*>> make_mems(const std::vector<mem_cfg_t> &layout);

static DifftestRef *ref = nullptr;

DifftestRef::DifftestRef() :
  cfg(create_cfg()),
  mems(make_mems(cfg->mem_layout())),
  plugin_devices(create_devices()),
  sim(create_sim(cfg)),
  p(sim->get_core(0UL)),
  state(p->get_state()) {
}

DifftestRef::~DifftestRef() {
  delete cfg;
  for (const auto& pair : mems) {
    delete pair.second;
  }
#ifdef CONFIG_DIFF_DEBUG_MODE
  delete plugin_devices.front().second;
#endif
  delete sim;
}

void DifftestRef::step(uint64_t n) {
  sim->step(n);
}

void DifftestRef::get_regs(diff_context_t *ctx) {
  for (int i = 0; i < NXPR; i++) {
    ctx->gpr[i] = state->XPR[i];
  }
#ifdef CONFIG_DIFF_FPU
  for (int i = 0; i < NFPR; i++) {
    ctx->fpr[i] = unboxF64(state->FPR[i]);
  }
#endif
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
#ifdef DIFF_DEBUG_MODE
  ctx->debugMode = state->debug_mode;
  ctx->dcsr = state->dcsr->read();
  ctx->dpc = state->dpc->read();
  ctx->dscratch0 = state->csrmap[CSR_DSCRATCH0]->read();
  ctx->dscratch1 = state->csrmap[CSR_DSCRATCH1]->read();
#endif // DIFF_DEBUG_MODE
}

void DifftestRef::set_regs(diff_context_t *ctx) {
  for (int i = 0; i < NXPR; i++) {
    state->XPR.write(i, ctx->gpr[i]);
  }
#ifdef CONFIG_DIFF_FPU
  for (int i = 0; i < NFPR; i++) {
    state->FPR.write(i, freg(f64(ctx->fpr[i])));
  }
#endif
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
#ifdef DIFF_DEBUG_MODE
  state->debug_mode = ctx->debugMode;
  state->dcsr->write(ctx->dcsr);
  state->dpc->write(ctx->dpc);
  state->csrmap[CSR_DSCRATCH0]->write(ctx->dscratch0);
  state->csrmap[CSR_DSCRATCH1]->write(ctx->dscratch1);
#endif // DIFF_DEBUG_MODE
}

void DifftestRef::memcpy_from_dut(reg_t dest, void* src, size_t n) {
  while (n) {
    char *base = sim->addr_to_mem(dest);
    size_t n_bytes = (n > PGSIZE) ? PGSIZE : n;
    memcpy(base, src, n_bytes);
    dest += PGSIZE;
    src = (char *)src + PGSIZE;
    n -= n_bytes;
  }
}

void DifftestRef::debug_memcpy_from_dut(reg_t dest, void* src, size_t n) {
#ifdef CONFIG_DIFF_DEBUG_MODE
  // addr is absolute physical addr
  // now we are not using this right now because I don't know how spike does if from device
  dummy_debug_t* dummy_debug = (dummy_debug_t *)plugin_devices.front().second;
  if (!dummy_debug) {
    return;
  }
  // start addr is virtual addr used by simulator
  int offset = (dest - DM_BASE_ADDR) / sizeof(uint8_t);
  uint8_t* start_addr = dummy_debug->dummy_debug_mem + offset;
  memcpy(start_addr, src, n); // TODO copy to device not addr
#else
#endif
}

int DifftestRef::store_commit(uint64_t *addr, uint64_t *data, uint8_t *mask) {
  return sim->dut_store_commit(addr, data, mask);
}

void DifftestRef::raise_intr(uint64_t no) {
  // Debug Intr
  if (no == 0xc) {
    p->halt_request = p->HR_REGULAR;
    step(0);
    p->halt_request = p->HR_NONE;
  } else {
    uint64_t mip_bit = 0x1UL << (no & 0xf);
    bool is_timer_interrupt = mip_bit & 0xa0UL;
    bool is_external_interrupt = mip_bit & 0xb00UL;
    bool from_outside = !(mip_bit & state->mip->read());
    bool external_set = (is_timer_interrupt || is_external_interrupt) && from_outside;
    if (external_set) {
      state->mip->backdoor_write_with_mask(mip_bit, mip_bit);
      step(1);
      state->mip->backdoor_write_with_mask(mip_bit, ~mip_bit);
    } else {
      step(1);
    }
  }
}

void DifftestRef::display() {
    int i;
  for (i = 0; i < 32; i ++) {
    printf("%4s: " FMT_WORD " ", xpr_name[i], state->XPR[i]);
    if (i % 4 == 3) {
      printf("\n");
    }
  }
  for (i = 0; i < 32; i ++) {
    printf("%4s: " FMT_WORD " ", fpr_name[i], unboxF64(state->FPR[i]));
    if (i % 4 == 3) {
      printf("\n");
    }
  }
  printf("pc: " FMT_WORD " mstatus: " FMT_WORD " mcause: " FMT_WORD " mepc: " FMT_WORD "\n",
      state->pc, state->mstatus->read(), state->mcause->read(), state->mepc->read());
  printf("%22s sstatus: " FMT_WORD " scause: " FMT_WORD " sepc: " FMT_WORD "\n",
      "", state->sstatus->read(), state->scause->read(), state->sepc->read());
  printf("satp: " FMT_WORD "\n", state->satp->read());
  printf("mip: " FMT_WORD " mie: " FMT_WORD " mscratch: " FMT_WORD " sscratch: " FMT_WORD "\n",
      state->mip->read(), state->mie->read(), state->csrmap[CSR_MSCRATCH]->read(), state->csrmap[CSR_MSCRATCH]->read());
  printf("mideleg: " FMT_WORD " medeleg: " FMT_WORD "\n",
      state->mideleg->read(), state->medeleg->read());
  printf("mtval: " FMT_WORD " stval: " FMT_WORD " mtvec: " FMT_WORD " stvec: " FMT_WORD "\n",
      state->mtval->read(), state->stval->read(), state->mtvec->read(), state->stvec->read());
  printf("privilege mode:%ld\n", state->prv);
  fflush(stdout);
}

const cfg_t *DifftestRef::create_cfg() {
  auto memory_layout = std::vector<mem_cfg_t>{
    mem_cfg_t{DRAM_BASE, CONFIG_MEMORY_SIZE},
  };
  auto const cfg = new cfg_t(
    // std::pair<reg_t, reg_t> default_initrd_bounds,
    std::make_pair(0, 0),
    // const char *default_bootargs,
    nullptr,
    // const char *default_isa,
    CONFIG_DIFF_ISA_STRING,
    // const char *default_priv
    DEFAULT_PRIV,
    // const char *default_varch,
    DEFAULT_VARCH,
    // const bool default_misaligned,
    false,
    // const endianness_t default_endianness,
    endianness_little,
    // const reg_t default_pmpregions,
    0,
    // const std::vector<mem_cfg_t> &default_mem_layout,
    memory_layout,
    // const std::vector<size_t> default_hartids,
    std::vector<size_t>{0},
    // bool default_real_time_clint,
    false,
    // const reg_t default_trigger_count
    0
  );
  return cfg;
}

const std::vector<std::pair<reg_t, abstract_device_t*>> DifftestRef::create_devices() {
#if defined(CONFIG_FLASH_BASE) && defined(CONFIG_FLASH_SIZE)
  // Initialize the flash with preset instructions
  const uint32_t flash_init[] = {
    0x0010029bUL, // CONFIG_FLASH_SIZE + 0: addiw t0, zero, 1
    0x01f29293UL, // CONFIG_FLASH_SIZE + 4: slli  t0, t0, 0x1f
    0x00028067UL, // CONFIG_FLASH_SIZE + 8: jr    t0
  };
  std::vector<char> rom_data((char*)flash_init, (char*)flash_init + sizeof(flash_init));
  rom_data.resize(CONFIG_FLASH_SIZE, 0);
#endif
  return std::vector<std::pair<reg_t, abstract_device_t*>>{
#ifdef CONFIG_DIFF_DEBUG_MODE
    std::make_pair(reg_t(DM_BASE_ADDR), new dummy_debug_t),
#endif
#if defined(CONFIG_FLASH_BASE) && defined(CONFIG_FLASH_SIZE)
    std::make_pair(reg_t(0x10000000UL), new rom_device_t(rom_data)),
#endif
  };
}

sim_t *DifftestRef::create_sim(const cfg_t *cfg) {
  sim_t *s = new sim_t(
    // const cfg_t *cfg,
    cfg,
    // bool halted,
    false,
    // std::vector<std::pair<reg_t, mem_t*>> mems
    mems,
    // std::vector<std::pair<reg_t, abstract_device_t*>> plugin_devices
    plugin_devices,
    // const std::vector<std::string>& args
    std::vector<std::string>{},
    // const debug_module_config_t &dm_config
    difftest_dm_config,
    // const char *log_path
#ifndef LOG_PATH
    nullptr,
#else
    LOG_PATH,
#endif // LOG_PATH
    //bool dtb_enabled, const char *dtb_file, bool socket_enabled, FILE *cmd_file
    false, nullptr, false, nullptr
  );

  return s;
}

// Following are the interfaces for co-simulation with other designs

extern "C" {

void difftest_memcpy(uint64_t addr, void *buf, size_t n, bool direction) {
  if (direction == DIFFTEST_TO_REF) {
    ref->memcpy_from_dut(addr, buf, n);
  } else {
    printf("difftest_memcpy with DIFFTEST_TO_DUT is not supported yet\n");
    fflush(stdout);
    assert(0);
  }
}

void difftest_regcpy(diff_context_t* dut, bool direction) {
  if (direction == DIFFTEST_TO_REF) {
    ref->set_regs(dut);
  } else {
    ref->get_regs(dut);
  }
}

void difftest_csrcpy(void *dut, bool direction) {

}

void difftest_uarchstatus_sync(void *dut) {
  ref->update_uarch_status(dut);
}

void update_dynamic_config(void* config) {
  ref->update_dynamic_config(config);
}

void difftest_exec(uint64_t n) {
  ref->step(n);
}

void difftest_init(int port) {
  ref = new DifftestRef;
#ifdef RISCV_ENABLE_COMMITLOG
  setvbuf(p->get_log_file(), NULL, _IONBF, 0);
#endif
}

void difftest_raise_intr(uint64_t NO) {
  ref->raise_intr(NO);
}

void isa_reg_display() {
  ref->display();
}

int difftest_store_commit(uint64_t *addr, uint64_t *data, uint8_t *mask) {
  return ref->store_commit(addr, data, mask);
}

uint64_t difftest_guided_exec(void *) {
  ref->step(1);
  return 0;
}

void debug_mem_sync(reg_t addr, void* buf, size_t n) {
  ref->debug_memcpy_from_dut(addr, buf, n);
}

void difftest_load_flash(void *flash_bin, size_t size) {

}


void difftest_close() {
  delete ref;
}

}
