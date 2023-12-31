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
extern std::vector<std::pair<reg_t, abstract_mem_t*>> make_mems(const std::vector<mem_cfg_t> &layout);

static DifftestRef *ref = nullptr;
static size_t overrided_mem_size = 0;
static size_t overrided_mhartid = 0;

DifftestRef::DifftestRef() :
  cfg(create_cfg()),
  mems(make_mems(cfg->mem_layout)),
  plugin_devices(create_devices()),
  sim(create_sim(cfg)),
  p(sim->get_core(0UL)),
  state(p->get_state()) {
#if CONFIG_PMP_NUM > 0
  p->set_pmp_granularity(1 << CONFIG_PMP_GRAN);
#endif
}

DifftestRef::~DifftestRef() {
  delete cfg;
  for (const auto& pair : mems) {
    delete pair.second;
  }
  delete sim;
}

void DifftestRef::step(uint64_t n) {
  sim->step(n);
}

void DifftestRef::skip_one(bool isRVC, bool wen, uint32_t wdest, uint64_t wdata) {
  state->pc += isRVC ? 2 : 4;
  // TODO: what if skip with fpwen?
  if (wen) {
    state->XPR.write(wdest, wdata);
  }
  // minstret decrements itself when written to match Spike's automated increment.
  // Therefore, we need to add + 2 here.
  state->minstret->write(state->minstret->read() + 2);
}

void DifftestRef::get_regs(diff_context_t *ctx) {
  // INFO: SPIKE's state_t struct and its reset function sucks.
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
  ctx->sstatus = state->nonvirtual_sstatus->read();
  ctx->scause = state->nonvirtual_scause->read();
  ctx->sepc = state->nonvirtual_sepc->read();
  ctx->satp = state->nonvirtual_satp->read();
  ctx->mip = state->mip->read();
  ctx->mie = state->mie->read();
  ctx->mscratch = state->mscratch->read();
  ctx->sscratch = state->nonvirtual_sscratch->read();
  ctx->mideleg = state->mideleg->read();
  ctx->medeleg = state->medeleg->read();
  ctx->mtval = state->mtval->read();
  ctx->stval = state->nonvirtual_stval->read();
  ctx->mtvec = state->mtvec->read();
  ctx->stvec = state->nonvirtual_stvec->read();
  ctx->priv = state->prv;
  // Some of above CSRs need to use nonvirtual_ type, as SPIKE redirect them
  // when virtualization is on (V = 1).
  // Some nonvirtual_ CSRs are added by XiangShan Difftest.

#ifdef DIFF_DEBUG_MODE
  ctx->debugMode = state->debug_mode;
  ctx->dcsr = state->dcsr->read();
  ctx->dpc = state->dpc->read();
  ctx->dscratch0 = state->csrmap[CSR_DSCRATCH0]->read();
  ctx->dscratch1 = state->csrmap[CSR_DSCRATCH1]->read();
#endif // DIFF_DEBUG_MODE

#ifdef CONFIG_DIFF_RVH
  ctx->v = state->v;
  ctx->mtval2 = state->mtval2->read();
  ctx->mtinst = state->mtinst->read();
  ctx->hstatus = state->hstatus->read();
  ctx->hideleg = state->hideleg->read();
  ctx->hedeleg = state->hedeleg->read();
  ctx->hcounteren = state->hcounteren->read();
  ctx->htval = state->htval->read();
  ctx->htinst = state->htinst->read();
  ctx->hgatp = state->hgatp->read();
  ctx->vsstatus = state->vsstatus->read();
  ctx->vstvec = state->vstvec->read();
  ctx->vsepc = state->vsepc->read();
  ctx->vscause = state->vscause->read();
  ctx->vstval = state->vstval->read();
  ctx->vsatp = state->vsatp->read();
  ctx->vsscratch = state->vsscratch->read();
#endif

#ifdef CONFIG_DIFF_RVV
  auto& vstate = p->VU;
  /*******************************ONLY FOR VLEN=128,ELEN=64*******************************************/
  for(int i = 0; i < NVPR; i++){
    auto vReg_Val0 = vstate.elt<uint64_t>(i, 0,false);
    auto vReg_Val1 = vstate.elt<uint64_t>(i, 1,false);
    ctx->vr[i]._64[0] = vReg_Val0;
    ctx->vr[i]._64[1] = vReg_Val1;
  }
  /***************************************************************************************************/
  ctx->vstart     = vstate.vstart->read();
  ctx->vxsat      = vstate.vxsat->read();
  ctx->vxrm       = vstate.vxrm->read();
  ctx->vcsr       = state->csrmap[CSR_VCSR]->read();
  ctx->vl         = vstate.vl->read();
  ctx->vtype      = vstate.vtype->read();
  ctx->vlenb      = vstate.vlenb;
#endif // CONFIG_DIFF_RVV
}

void DifftestRef::set_regs(diff_context_t *ctx, bool on_demand) {
  for (int i = 0; i < NXPR; i++) {
    if (!on_demand || state->XPR[i] != ctx->gpr[i]) {
      state->XPR.write(i, ctx->gpr[i]);
    }
  }
#ifdef CONFIG_DIFF_FPU
  for (int i = 0; i < NFPR; i++) {
    if (!on_demand || unboxF64(state->FPR[i]) != ctx->fpr[i]) {
      state->FPR.write(i, freg(f64(ctx->fpr[i])));
    }
  }
#endif
  if (!on_demand || state->pc != ctx->pc) {
    state->pc = ctx->pc;
  }
  if (!on_demand || state->mstatus->read() != ctx->mstatus) {
    state->mstatus->write(ctx->mstatus);
  }
  if (!on_demand || state->mcause->read() != ctx->mcause) {
    state->mcause->write(ctx->mcause);
  }
  if (!on_demand || state->mepc->read() != ctx->mepc) {
    state->mepc->write(ctx->mepc);
  }
  if (!on_demand || state->nonvirtual_sstatus->read() != ctx->sstatus) {
    state->nonvirtual_sstatus->write(ctx->sstatus);
  }
  if (!on_demand || state->nonvirtual_scause->read() != ctx->scause) {
    state->nonvirtual_scause->write(ctx->scause);
  }
  if (!on_demand || state->nonvirtual_sepc->read() != ctx->sepc) {
    state->nonvirtual_sepc->write(ctx->sepc);
  }
  if (!on_demand || state->nonvirtual_satp->read() != ctx->satp) {
    state->nonvirtual_satp->write(ctx->satp);
  }
  if (!on_demand || state->mip->read() != ctx->mip) {
    state->mip->write(ctx->mip);
  }
  if (!on_demand || state->mie->read() != ctx->mie) {
    state->mie->write(ctx->mie);
  }
  if (!on_demand || state->mscratch->read() != ctx->mscratch) {
    state->mscratch->write(ctx->mscratch);
  }
  if (!on_demand || state->nonvirtual_sscratch->read() != ctx->sscratch) {
    state->nonvirtual_sscratch->write(ctx->sscratch);
  }
  if (!on_demand || state->mideleg->read() != ctx->mideleg) {
    state->mideleg->write(ctx->mideleg);
  }
  if (!on_demand || state->medeleg->read() != ctx->medeleg) {
    state->medeleg->write(ctx->medeleg);
  }
  if (!on_demand || state->mtval->read() != ctx->mtval) {
    state->mtval->write(ctx->mtval);
  }
  if (!on_demand || state->stval->read() != ctx->stval) {
    state->stval->write(ctx->stval);
  }
  if (!on_demand || state->mtvec->read() != ctx->mtvec) {
    state->mtvec->write(ctx->mtvec);
  }
  if (!on_demand || state->nonvirtual_stvec->read() != ctx->stvec) {
    state->nonvirtual_stvec->write(ctx->stvec);
  }
  if (!on_demand || state->prv != ctx->priv) {
    state->prv = ctx->priv;
  }
#ifdef DIFF_DEBUG_MODE
  if (!on_demand || state->debug_mode->read() != ctx->debugMode) {
    state->debug_mode = ctx->debugMode;
  }
  if (!on_demand || state->dcsr->read() != ctx->dcsr) {
    state->dcsr->write(ctx->dcsr);
  }
  if (!on_demand || state->dpc->read() != ctx->dpc) {
    state->dpc->write(ctx->dpc);
  }
  if (!on_demand || state->csrmap[CSR_DSCRATCH0]->read() != ctx->dscratch0) {
    state->csrmap[CSR_DSCRATCH0]->write(ctx->dscratch0);
  }
  if (!on_demand || state->csrmap[CSR_DSCRATCH1]->read() != ctx->dscratch1) {
    state->csrmap[CSR_DSCRATCH1]->write(ctx->dscratch1);
  }
#endif // DIFF_DEBUG_MODE

#ifdef CONFIG_DIFF_RVH
  if (!on_demand || state->v != ctx->v) {
    state->v = ctx->v;
  }
  if (!on_demand || state->mtval2->read() != ctx->mtval2) {
    state->mtval2->write(ctx->mtval2);
  }
  if (!on_demand || state->mtinst->read() != ctx->mtinst) {
    state->mtinst->write(ctx->mtinst);
  }
  if (!on_demand || state->hstatus->read() != ctx->hstatus) {
    state->hstatus->write(ctx->hstatus);
  }
  if (!on_demand || state->hideleg->read() != ctx->hideleg) {
    state->hideleg->write(ctx->hideleg);
  }
  if (!on_demand || state->hedeleg->read() != ctx->hedeleg) {
    state->hedeleg->write(ctx->hedeleg);
  }
  if (!on_demand || state->hcounteren->read() != ctx->hcounteren) {
    state->hcounteren->write(ctx->hcounteren);
  }
  if (!on_demand || state->htval->read() != ctx->htval) {
    state->htval->write(ctx->htval);
  }
  if (!on_demand || state->htinst->read() != ctx->htinst) {
    state->htinst->write(ctx->htinst);
  }
  if (!on_demand || state->hgatp->read() != ctx->hgatp) {
    state->hgatp->write(ctx->hgatp);
  }
  if (!on_demand || state->vsstatus->read() != ctx->vsstatus) {
    state->vsstatus->write(ctx->vsstatus);
  }
  if (!on_demand || state->vstvec->read() != ctx->vstvec) {
    state->vstvec->write(ctx->vstvec);
  }
  if (!on_demand || state->vsepc->read() != ctx->vsepc) {
    state->vsepc->write(ctx->vsepc);
  }
  if (!on_demand || state->vscause->read() != ctx->vscause) {
    state->vscause->write(ctx->vscause);
  }
  if (!on_demand || state->vstval->read() != ctx->vstval) {
    state->vstval->write(ctx->vstval);
  }
  if (!on_demand || state->vsatp->read() != ctx->vsatp) {
    state->vsatp->write(ctx->vsatp);
  }
  if (!on_demand || state->csrmap[CSR_VSSCRATCH]->read() != ctx->vsscratch) {
    state->csrmap[CSR_VSSCRATCH]->write(ctx->vsscratch);
  }
#endif // CONFIG_DIFF_RVH

#ifdef CONFIG_DIFF_RVV
  auto& vstate = p->VU;
  /**********************ONLY FOR VLEN=128,ELEN=64************************************/
  for (int i = 0; i < NVPR; i++) {
    auto &vReg_Val0 = p->VU.elt<uint64_t>(i, 0, true);
    auto &vReg_Val1 = p->VU.elt<uint64_t>(i, 1, true);
    if (!on_demand || vReg_Val0 != ctx->vr[i]._64[0]) {
      vReg_Val0 = ctx->vr[i]._64[0];
    }
    if(!on_demand || vReg_Val1 != ctx->vr[i]._64[1]){
      vReg_Val1 = ctx->vr[i]._64[1];
    }
  }
  /***********************************************************************************/
  if (!on_demand || vstate.vstart->read() != ctx->vstart) {
    vstate.vstart->write_raw(ctx->vstart);
  }
  /**********************NEED TO ADD WRITE*********************************************/
  if (!on_demand || vstate.vxsat->read() != ctx->vxsat) {
    // vstate.vxsat->write(ctx->vxsat);
  }
  if (!on_demand || vstate.vxrm->read() != ctx->vxrm) {
    vstate.vxrm->write_raw(ctx->vxrm);
  }
  /******************************Don't need write vcsr**********************************/
  // if (!on_demand || state->csrmap[CSR_VCSR]->read() !=ctx->vcsr) {
  //   csrmap[CSR_VCSR]->write(ctx->vcsr);
  // }
  if (!on_demand || vstate.vl->read() != ctx->vl) {
    vstate.vl->write_raw(ctx->vl);
  }
  if (!on_demand || vstate.vtype->read() != ctx->vtype) {
    vstate.vtype->write_raw(ctx->vtype);
  }
  if (!on_demand || vstate.vlenb != ctx->vlenb) {
    vstate.vlenb = ctx->vlenb;
  }
#endif // CONFIG_DIFF_RVV
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
  for (int i = 0; i < CONFIG_PMP_NUM; i++) {
    auto cfgidx = i / 4;
    if (p->get_xlen() == 64) {
      cfgidx -= cfgidx % 2;
    }
    unsigned pmpcfg = (state->csrmap[CSR_PMPCFG0 + cfgidx]->read() >> (i % (p->get_xlen() / 8)) * 8) & 0xffU;
    printf("%2d: cfg:0x%02x addr:0x%016lx", i, pmpcfg, state->pmpaddr[i]->read());
    if (i % 2 == 1) printf("\n");
    else printf(" | ");
  }
  fflush(stdout);
}

const cfg_t *DifftestRef::create_cfg() {
  auto mem_size = overrided_mem_size ? overrided_mem_size : CONFIG_MEMORY_SIZE;
  auto memory_layout = std::vector<mem_cfg_t>{
    mem_cfg_t{DRAM_BASE, mem_size},
  };
  auto const cfg = new cfg_t();
  cfg->initrd_bounds = std::make_pair(0, 0);
  cfg->bootargs = nullptr;
  cfg->isa = CONFIG_DIFF_ISA_STRING;
  cfg->priv = DEFAULT_PRIV;
  cfg->varch = DEFAULT_VARCH;
  cfg->misaligned = false;
    // const endianness_t default_endianness,
  cfg->endianness = endianness_little;
  cfg->pmpregions = CONFIG_PMP_NUM;
  cfg->mem_layout = memory_layout;
  cfg->hartids = std::vector<size_t>{overrided_mhartid};
  cfg->real_time_clint = false;
  cfg->trigger_count = 0;
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
    std::make_pair(reg_t(CONFIG_FLASH_BASE), new rom_device_t(rom_data)),
#endif
  };
}

sim_t *DifftestRef::create_sim(const cfg_t *cfg) {
  sim_t *s = new sim_t(
    // const cfg_t *cfg,
    cfg,
    // bool halted,
    false,
    // std::vector<std::pair<reg_t, abstract_mem_t*>> mems
    mems,
    // std::vector<device_factory_t*> plugin_device_factories
    std::vector<device_factory_t*>{},
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

  for (const auto& pair : plugin_devices) {
    s->add_device(pair.first, std::shared_ptr<abstract_device_t>(pair.second));
  }

  return s;
}

// Following are the interfaces for co-simulation with other designs

extern "C" {

int difftest_disambiguation_state() {
  return ref->disambiguation_state();
}

void difftest_memcpy(uint64_t addr, void *buf, size_t n, bool direction) {
  if (direction == DIFFTEST_TO_REF) {
    ref->memcpy_from_dut(addr, buf, n);
  } else {
    printf("difftest_memcpy with DIFFTEST_TO_DUT is not supported yet\n");
    fflush(stdout);
    assert(0);
  }
}

void difftest_regcpy(diff_context_t* dut, bool direction, bool on_demand) {
  if (direction == DIFFTEST_TO_REF) {
    ref->set_regs(dut, on_demand);
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

void difftest_skip_one(bool isRVC, bool wen, uint32_t wdest, uint64_t wdata) {
  ref->skip_one(isRVC, wen, wdest, wdata);
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

void difftest_display() {
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

void difftest_set_mhartid(int mhartid) {
  overrided_mhartid = mhartid;
}

void difftest_close() {
  delete ref;
}

void difftest_set_ramsize(size_t size) {
  overrided_mem_size = size;
}

}
