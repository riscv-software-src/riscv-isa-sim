#include "processor.h"
#include "debug_defines.h"

void state_t::add_csr(reg_t addr, const csr_t_p& csr)
{
  csrmap[addr] = csr;
}

#define add_const_ext_csr(ext, addr, csr) do { auto csr__ = (csr); if (proc->extension_enabled_const(ext)) { add_csr(addr, csr__); } } while (0)
#define add_ext_csr(ext, addr, csr) do { auto csr__ = (csr); if (proc->extension_enabled(ext)) { add_csr(addr, csr__); } } while (0)
#define add_user_csr(addr, csr) add_const_ext_csr('U', addr, csr)
#define add_supervisor_csr(addr, csr) add_const_ext_csr('S', addr, csr)
#define add_hypervisor_csr(addr, csr) add_ext_csr('H', addr, csr)

void state_t::csr_init(processor_t* const proc, reg_t max_isa)
{
  // This assumes xlen is always max_xlen, which is true today (see
  // mstatus_csr_t::unlogged_write()):
  auto xlen = proc->get_isa().get_max_xlen();

  add_csr(CSR_MISA, misa = std::make_shared<misa_csr_t>(proc, CSR_MISA, max_isa));
  mstatus = std::make_shared<mstatus_csr_t>(proc, CSR_MSTATUS);

  if (xlen == 32) {
    add_csr(CSR_MSTATUS, std::make_shared<rv32_low_csr_t>(proc, CSR_MSTATUS, mstatus));
    add_csr(CSR_MSTATUSH, mstatush = std::make_shared<rv32_high_csr_t>(proc, CSR_MSTATUSH, mstatus));
  } else {
    add_csr(CSR_MSTATUS, mstatus);
  }
  add_csr(CSR_MEPC, mepc = std::make_shared<epc_csr_t>(proc, CSR_MEPC));
  add_csr(CSR_MTVAL, mtval = std::make_shared<basic_csr_t>(proc, CSR_MTVAL, 0));
  add_csr(CSR_MSCRATCH, std::make_shared<basic_csr_t>(proc, CSR_MSCRATCH, 0));
  add_csr(CSR_MTVEC, mtvec = std::make_shared<tvec_csr_t>(proc, CSR_MTVEC));
  add_csr(CSR_MCAUSE, mcause = std::make_shared<cause_csr_t>(proc, CSR_MCAUSE));

  const reg_t minstretcfg_mask = !proc->extension_enabled_const(EXT_SMCNTRPMF) ? 0 :
    MHPMEVENT_MINH | MHPMEVENT_SINH | MHPMEVENT_UINH | MHPMEVENT_VSINH | MHPMEVENT_VUINH;
  auto minstretcfg = std::make_shared<smcntrpmf_csr_t>(proc, CSR_MINSTRETCFG, minstretcfg_mask, 0);
  auto mcyclecfg = std::make_shared<smcntrpmf_csr_t>(proc, CSR_MCYCLECFG, minstretcfg_mask, 0);

  minstret = std::make_shared<wide_counter_csr_t>(proc, CSR_MINSTRET, minstretcfg);
  mcycle = std::make_shared<wide_counter_csr_t>(proc, CSR_MCYCLE, mcyclecfg);
  time = std::make_shared<time_counter_csr_t>(proc, CSR_TIME);
  if (proc->extension_enabled_const(EXT_ZICNTR)) {
    add_csr(CSR_INSTRET, std::make_shared<counter_proxy_csr_t>(proc, CSR_INSTRET, minstret));
    add_csr(CSR_CYCLE, std::make_shared<counter_proxy_csr_t>(proc, CSR_CYCLE, mcycle));
    add_csr(CSR_TIME, time_proxy = std::make_shared<counter_proxy_csr_t>(proc, CSR_TIME, time));
  }
  if (xlen == 32) {
    csr_t_p minstreth, mcycleh;
    add_csr(CSR_MINSTRET, std::make_shared<rv32_low_csr_t>(proc, CSR_MINSTRET, minstret));
    add_csr(CSR_MINSTRETH, minstreth = std::make_shared<rv32_high_csr_t>(proc, CSR_MINSTRETH, minstret));
    add_csr(CSR_MCYCLE, std::make_shared<rv32_low_csr_t>(proc, CSR_MCYCLE, mcycle));
    add_csr(CSR_MCYCLEH, mcycleh = std::make_shared<rv32_high_csr_t>(proc, CSR_MCYCLEH, mcycle));
    if (proc->extension_enabled_const(EXT_ZICNTR)) {
      auto timeh = std::make_shared<rv32_high_csr_t>(proc, CSR_TIMEH, time);
      add_csr(CSR_INSTRETH, std::make_shared<counter_proxy_csr_t>(proc, CSR_INSTRETH, minstreth));
      add_csr(CSR_CYCLEH, std::make_shared<counter_proxy_csr_t>(proc, CSR_CYCLEH, mcycleh));
      add_csr(CSR_TIMEH, std::make_shared<counter_proxy_csr_t>(proc, CSR_TIMEH, timeh));
    }
  } else {
    add_csr(CSR_MINSTRET, minstret);
    add_csr(CSR_MCYCLE, mcycle);
  }
  for (reg_t i = 0; i < N_HPMCOUNTERS; ++i) {
    const reg_t which_mevent = CSR_MHPMEVENT3 + i;
    const reg_t which_meventh = CSR_MHPMEVENT3H + i;
    const reg_t which_mcounter = CSR_MHPMCOUNTER3 + i;
    const reg_t which_mcounterh = CSR_MHPMCOUNTER3H + i;
    const reg_t which_counter = CSR_HPMCOUNTER3 + i;
    const reg_t which_counterh = CSR_HPMCOUNTER3H + i;
    mevent[i] = std::make_shared<mevent_csr_t>(proc, which_mevent);
    auto mcounter = std::make_shared<const_csr_t>(proc, which_mcounter, 0);
    add_csr(which_mcounter, mcounter);

    auto counter = std::make_shared<counter_proxy_csr_t>(proc, which_counter, mcounter);
    add_const_ext_csr(EXT_ZIHPM, which_counter, counter);

    if (xlen == 32) {
      add_csr(which_mevent, std::make_shared<rv32_low_csr_t>(proc, which_mevent, mevent[i]));
      auto mcounterh = std::make_shared<const_csr_t>(proc, which_mcounterh, 0);
      add_csr(which_mcounterh, mcounterh);
      add_const_ext_csr(EXT_ZIHPM, which_counterh, std::make_shared<counter_proxy_csr_t>(proc, which_counterh, mcounterh));
      add_const_ext_csr(EXT_SSCOFPMF, which_meventh, std::make_shared<rv32_high_csr_t>(proc, which_meventh, mevent[i]));
    } else {
      add_csr(which_mevent, mevent[i]);
    }
  }
  add_csr(CSR_MCOUNTINHIBIT, std::make_shared<const_csr_t>(proc, CSR_MCOUNTINHIBIT, 0));
  add_const_ext_csr(EXT_SSCOFPMF, CSR_SCOUNTOVF, std::make_shared<scountovf_csr_t>(proc, CSR_SCOUNTOVF));
  add_csr(CSR_MIE, mie = std::make_shared<mie_csr_t>(proc, CSR_MIE));
  add_csr(CSR_MIP, mip = std::make_shared<mip_csr_t>(proc, CSR_MIP));
  auto sip_sie_accr = std::make_shared<generic_int_accessor_t>(
    this,
    ~MIP_HS_MASK,  // read_mask
    MIP_SSIP | MIP_LCOFIP,  // ip_write_mask
    ~MIP_HS_MASK,  // ie_write_mask
    generic_int_accessor_t::mask_mode_t::MIDELEG,
    0              // shiftamt
  );

  auto hip_hie_accr = std::make_shared<generic_int_accessor_t>(
    this,
    MIP_HS_MASK,   // read_mask
    MIP_VSSIP,     // ip_write_mask
    MIP_HS_MASK,   // ie_write_mask
    generic_int_accessor_t::mask_mode_t::MIDELEG,
    0              // shiftamt
  );

  auto vsip_vsie_accr = std::make_shared<generic_int_accessor_t>(
    this,
    MIP_VS_MASK,   // read_mask
    MIP_VSSIP,     // ip_write_mask
    MIP_VS_MASK,   // ie_write_mask
    generic_int_accessor_t::mask_mode_t::HIDELEG,
    1              // shiftamt
  );

  auto nonvirtual_sip = std::make_shared<mip_proxy_csr_t>(proc, CSR_SIP, sip_sie_accr);
  auto vsip = std::make_shared<mip_proxy_csr_t>(proc, CSR_VSIP, vsip_vsie_accr);
  add_hypervisor_csr(CSR_VSIP, vsip);
  add_supervisor_csr(CSR_SIP, std::make_shared<virtualized_csr_t>(proc, nonvirtual_sip, vsip));
  add_hypervisor_csr(CSR_HIP, std::make_shared<mip_proxy_csr_t>(proc, CSR_HIP, hip_hie_accr));
  add_hypervisor_csr(CSR_HVIP, hvip = std::make_shared<hvip_csr_t>(proc, CSR_HVIP, 0));

  auto nonvirtual_sie = std::make_shared<mie_proxy_csr_t>(proc, CSR_SIE, sip_sie_accr);
  auto vsie = std::make_shared<mie_proxy_csr_t>(proc, CSR_VSIE, vsip_vsie_accr);
  add_hypervisor_csr(CSR_VSIE, vsie);
  add_supervisor_csr(CSR_SIE, std::make_shared<virtualized_csr_t>(proc, nonvirtual_sie, vsie));
  add_hypervisor_csr(CSR_HIE, std::make_shared<mie_proxy_csr_t>(proc, CSR_HIE, hip_hie_accr));

  add_supervisor_csr(CSR_MEDELEG, medeleg = std::make_shared<medeleg_csr_t>(proc, CSR_MEDELEG));
  add_supervisor_csr(CSR_MIDELEG, mideleg = std::make_shared<mideleg_csr_t>(proc, CSR_MIDELEG));
  const reg_t counteren_mask = (proc->extension_enabled_const(EXT_ZICNTR) ? 0x7UL : 0x0) | (proc->extension_enabled_const(EXT_ZIHPM) ? 0xfffffff8ULL : 0x0);
  add_user_csr(CSR_MCOUNTEREN, mcounteren = std::make_shared<masked_csr_t>(proc, CSR_MCOUNTEREN, counteren_mask, 0));
  add_supervisor_csr(CSR_SCOUNTEREN, scounteren = std::make_shared<masked_csr_t>(proc, CSR_SCOUNTEREN, counteren_mask, 0));
  nonvirtual_sepc = std::make_shared<epc_csr_t>(proc, CSR_SEPC);
  add_hypervisor_csr(CSR_VSEPC, vsepc = std::make_shared<epc_csr_t>(proc, CSR_VSEPC));
  add_supervisor_csr(CSR_SEPC, sepc = std::make_shared<virtualized_csr_t>(proc, nonvirtual_sepc, vsepc));
  nonvirtual_stval = std::make_shared<basic_csr_t>(proc, CSR_STVAL, 0);
  add_hypervisor_csr(CSR_VSTVAL, vstval = std::make_shared<basic_csr_t>(proc, CSR_VSTVAL, 0));
  add_supervisor_csr(CSR_STVAL, stval = std::make_shared<virtualized_csr_t>(proc, nonvirtual_stval, vstval));
  auto sscratch = std::make_shared<basic_csr_t>(proc, CSR_SSCRATCH, 0);
  auto vsscratch = std::make_shared<basic_csr_t>(proc, CSR_VSSCRATCH, 0);
  // Note: if max_isa does not include H, we don't really need this virtualized_csr_t at all (though it doesn't hurt):
  add_supervisor_csr(CSR_SSCRATCH, std::make_shared<virtualized_csr_t>(proc, sscratch, vsscratch));
  add_hypervisor_csr(CSR_VSSCRATCH, vsscratch);
  nonvirtual_stvec = std::make_shared<tvec_csr_t>(proc, CSR_STVEC);
  add_hypervisor_csr(CSR_VSTVEC, vstvec = std::make_shared<tvec_csr_t>(proc, CSR_VSTVEC));
  add_supervisor_csr(CSR_STVEC, stvec = std::make_shared<virtualized_csr_t>(proc, nonvirtual_stvec, vstvec));
  auto nonvirtual_satp = std::make_shared<satp_csr_t>(proc, CSR_SATP);
  add_hypervisor_csr(CSR_VSATP, vsatp = std::make_shared<base_atp_csr_t>(proc, CSR_VSATP));
  add_supervisor_csr(CSR_SATP, satp = std::make_shared<virtualized_satp_csr_t>(proc, nonvirtual_satp, vsatp));
  nonvirtual_scause = std::make_shared<cause_csr_t>(proc, CSR_SCAUSE);
  add_hypervisor_csr(CSR_VSCAUSE, vscause = std::make_shared<cause_csr_t>(proc, CSR_VSCAUSE));
  add_supervisor_csr(CSR_SCAUSE, scause = std::make_shared<virtualized_csr_t>(proc, nonvirtual_scause, vscause));
  mtval2 = std::make_shared<mtval2_csr_t>(proc, CSR_MTVAL2);
  if (proc->extension_enabled('H') || proc->extension_enabled(EXT_SSDBLTRP))
    add_csr(CSR_MTVAL2, mtval2);
  add_hypervisor_csr(CSR_MTINST, mtinst = std::make_shared<hypervisor_csr_t>(proc, CSR_MTINST));
  add_hypervisor_csr(CSR_HSTATUS, hstatus = std::make_shared<hstatus_csr_t>(proc, CSR_HSTATUS));
  add_hypervisor_csr(CSR_HGEIE, std::make_shared<const_csr_t>(proc, CSR_HGEIE, 0));
  add_hypervisor_csr(CSR_HGEIP, std::make_shared<const_csr_t>(proc, CSR_HGEIP, 0));
  add_hypervisor_csr(CSR_HIDELEG, hideleg = std::make_shared<hideleg_csr_t>(proc, CSR_HIDELEG, mideleg));
  const reg_t hedeleg_mask =
    (1 << CAUSE_MISALIGNED_FETCH) |
    (1 << CAUSE_FETCH_ACCESS) |
    (1 << CAUSE_ILLEGAL_INSTRUCTION) |
    (1 << CAUSE_BREAKPOINT) |
    (1 << CAUSE_MISALIGNED_LOAD) |
    (1 << CAUSE_LOAD_ACCESS) |
    (1 << CAUSE_MISALIGNED_STORE) |
    (1 << CAUSE_STORE_ACCESS) |
    (1 << CAUSE_USER_ECALL) |
    (1 << CAUSE_FETCH_PAGE_FAULT) |
    (1 << CAUSE_LOAD_PAGE_FAULT) |
    (1 << CAUSE_STORE_PAGE_FAULT) |
    (1 << CAUSE_SOFTWARE_CHECK_FAULT) |
    (1 << CAUSE_HARDWARE_ERROR_FAULT);
  add_hypervisor_csr(CSR_HEDELEG, hedeleg = std::make_shared<masked_csr_t>(proc, CSR_HEDELEG, hedeleg_mask, 0));
  add_hypervisor_csr(CSR_HCOUNTEREN, hcounteren = std::make_shared<masked_csr_t>(proc, CSR_HCOUNTEREN, counteren_mask, 0));
  htimedelta = std::make_shared<basic_csr_t>(proc, CSR_HTIMEDELTA, 0);
  if (xlen == 32) {
    add_hypervisor_csr(CSR_HTIMEDELTA, std::make_shared<rv32_low_csr_t>(proc, CSR_HTIMEDELTA, htimedelta));
    add_hypervisor_csr(CSR_HTIMEDELTAH, std::make_shared<rv32_high_csr_t>(proc, CSR_HTIMEDELTAH, htimedelta));
  } else {
    add_hypervisor_csr(CSR_HTIMEDELTA, htimedelta);
  }
  add_hypervisor_csr(CSR_HTVAL, htval = std::make_shared<basic_csr_t>(proc, CSR_HTVAL, 0));
  add_hypervisor_csr(CSR_HTINST, htinst = std::make_shared<basic_csr_t>(proc, CSR_HTINST, 0));
  add_hypervisor_csr(CSR_HGATP, hgatp = std::make_shared<hgatp_csr_t>(proc, CSR_HGATP));
  nonvirtual_sstatus = std::make_shared<sstatus_proxy_csr_t>(proc, CSR_SSTATUS, mstatus);
  add_hypervisor_csr(CSR_VSSTATUS, vsstatus = std::make_shared<vsstatus_csr_t>(proc, CSR_VSSTATUS));
  add_supervisor_csr(CSR_SSTATUS, sstatus = std::make_shared<sstatus_csr_t>(proc, nonvirtual_sstatus, vsstatus));

  add_csr(CSR_DPC, dpc = std::make_shared<dpc_csr_t>(proc, CSR_DPC));
  add_csr(CSR_DSCRATCH0, std::make_shared<debug_mode_csr_t>(proc, CSR_DSCRATCH0));
  add_csr(CSR_DSCRATCH1, std::make_shared<debug_mode_csr_t>(proc, CSR_DSCRATCH1));
  add_csr(CSR_DCSR, dcsr = std::make_shared<dcsr_csr_t>(proc, CSR_DCSR));

  add_csr(CSR_TSELECT, tselect = std::make_shared<tselect_csr_t>(proc, CSR_TSELECT));
  if (proc->get_cfg().trigger_count > 0) {
    add_csr(CSR_TDATA1, std::make_shared<tdata1_csr_t>(proc, CSR_TDATA1));
    add_csr(CSR_TDATA2, tdata2 = std::make_shared<tdata2_csr_t>(proc, CSR_TDATA2));
    add_csr(CSR_TDATA3, std::make_shared<tdata3_csr_t>(proc, CSR_TDATA3));
    add_csr(CSR_TINFO, std::make_shared<tinfo_csr_t>(proc, CSR_TINFO));
    add_csr(CSR_TCONTROL, tcontrol = std::make_shared<masked_csr_t>(proc, CSR_TCONTROL, CSR_TCONTROL_MPTE | CSR_TCONTROL_MTE, 0));
  } else {
    add_csr(CSR_TDATA1, std::make_shared<const_csr_t>(proc, CSR_TDATA1, 0));
    add_csr(CSR_TDATA2, tdata2 = std::make_shared<const_csr_t>(proc, CSR_TDATA2, 0));
    add_csr(CSR_TDATA3, std::make_shared<const_csr_t>(proc, CSR_TDATA3, 0));
    add_csr(CSR_TINFO, std::make_shared<const_csr_t>(proc, CSR_TINFO, 0));
    add_csr(CSR_TCONTROL, tcontrol = std::make_shared<const_csr_t>(proc, CSR_TCONTROL, 0));
  }
  unsigned scontext_length = (xlen == 32 ? 16 : 32); // debug spec suggests 16-bit for RV32 and 32-bit for RV64
  add_supervisor_csr(CSR_SCONTEXT, scontext = std::make_shared<masked_csr_t>(proc, CSR_SCONTEXT, (reg_t(1) << scontext_length) - 1, 0));
  unsigned hcontext_length = (xlen == 32 ? 6 : 13) + (proc->extension_enabled('H') ? 1 : 0); // debug spec suggest 7-bit (6-bit) for RV32 and 14-bit (13-bit) for RV64 with (without) H extension
  auto hcontext = std::make_shared<masked_csr_t>(proc, CSR_HCONTEXT, (reg_t(1) << hcontext_length) - 1, 0);
  add_hypervisor_csr(CSR_HCONTEXT, hcontext);
  add_csr(CSR_MCONTEXT, mcontext = std::make_shared<proxy_csr_t>(proc, CSR_MCONTEXT, hcontext));
  add_csr(CSR_MSECCFG, mseccfg = std::make_shared<mseccfg_csr_t>(proc, CSR_MSECCFG));

  for (int i = 0; i < max_pmp; ++i) {
    add_csr(CSR_PMPADDR0 + i, pmpaddr[i] = std::make_shared<pmpaddr_csr_t>(proc, CSR_PMPADDR0 + i));
  }
  for (int i = 0; i < max_pmp; i += xlen / 8) {
    reg_t addr = CSR_PMPCFG0 + i / 4;
    add_csr(addr, std::make_shared<pmpcfg_csr_t>(proc, addr));
  }

  add_csr(CSR_FFLAGS, fflags = std::make_shared<float_csr_t>(proc, CSR_FFLAGS, FSR_AEXC >> FSR_AEXC_SHIFT, 0));
  add_csr(CSR_FRM, frm = std::make_shared<float_csr_t>(proc, CSR_FRM, FSR_RD >> FSR_RD_SHIFT, 0));
  assert(FSR_AEXC_SHIFT == 0);  // composite_csr_t assumes fflags begins at bit 0
  add_csr(CSR_FCSR, std::make_shared<composite_csr_t>(proc, CSR_FCSR, frm, fflags, FSR_RD_SHIFT));

  add_ext_csr(EXT_ZKR, CSR_SEED, std::make_shared<seed_csr_t>(proc, CSR_SEED));

  add_csr(CSR_MARCHID, std::make_shared<const_csr_t>(proc, CSR_MARCHID, 5));
  add_csr(CSR_MIMPID, std::make_shared<const_csr_t>(proc, CSR_MIMPID, 0));
  add_csr(CSR_MVENDORID, std::make_shared<const_csr_t>(proc, CSR_MVENDORID, 0));
  add_csr(CSR_MHARTID, std::make_shared<const_csr_t>(proc, CSR_MHARTID, proc->get_id()));
  add_csr(CSR_MCONFIGPTR, std::make_shared<const_csr_t>(proc, CSR_MCONFIGPTR, 0));
  const reg_t menvcfg_mask = (proc->extension_enabled(EXT_ZICBOM) ? MENVCFG_CBCFE | MENVCFG_CBIE : 0) |
                            (proc->extension_enabled(EXT_ZICBOZ) ? MENVCFG_CBZE : 0) |
                            (proc->extension_enabled(EXT_SMNPM) ? MENVCFG_PMM : 0) |
                            (proc->extension_enabled(EXT_SVADU) ? MENVCFG_ADUE: 0) |
                            (proc->extension_enabled(EXT_SVPBMT) ? MENVCFG_PBMTE : 0) |
                            (proc->extension_enabled(EXT_SSTC) ? MENVCFG_STCE : 0) |
                            (proc->extension_enabled(EXT_ZICFILP) ? MENVCFG_LPE : 0) |
                            (proc->extension_enabled(EXT_ZICFISS) ? MENVCFG_SSE : 0) |
                            (proc->extension_enabled(EXT_SSDBLTRP) ? MENVCFG_DTE : 0);
  menvcfg = std::make_shared<envcfg_csr_t>(proc, CSR_MENVCFG, menvcfg_mask, 0);
  if (xlen == 32) {
    add_user_csr(CSR_MENVCFG, std::make_shared<rv32_low_csr_t>(proc, CSR_MENVCFG, menvcfg));
    add_user_csr(CSR_MENVCFGH, std::make_shared<rv32_high_csr_t>(proc, CSR_MENVCFGH, menvcfg));
  } else {
    add_user_csr(CSR_MENVCFG, menvcfg);
  }
  const reg_t senvcfg_mask = (proc->extension_enabled(EXT_ZICBOM) ? SENVCFG_CBCFE | SENVCFG_CBIE : 0) |
                            (proc->extension_enabled(EXT_ZICBOZ) ? SENVCFG_CBZE : 0) |
                            (proc->extension_enabled(EXT_SSNPM) ? SENVCFG_PMM : 0) |
                            (proc->extension_enabled(EXT_ZICFILP) ? SENVCFG_LPE : 0) |
                            (proc->extension_enabled(EXT_ZICFISS) ? SENVCFG_SSE : 0);
  add_supervisor_csr(CSR_SENVCFG, senvcfg = std::make_shared<senvcfg_csr_t>(proc, CSR_SENVCFG, senvcfg_mask, 0));
  const reg_t henvcfg_mask = (proc->extension_enabled(EXT_ZICBOM) ? HENVCFG_CBCFE | HENVCFG_CBIE : 0) |
                            (proc->extension_enabled(EXT_ZICBOZ) ? HENVCFG_CBZE : 0) |
                            (proc->extension_enabled(EXT_SSNPM) ? HENVCFG_PMM : 0) |
                            (proc->extension_enabled(EXT_SVADU) ? HENVCFG_ADUE: 0) |
                            (proc->extension_enabled(EXT_SVPBMT) ? HENVCFG_PBMTE : 0) |
                            (proc->extension_enabled(EXT_SSTC) ? HENVCFG_STCE : 0) |
                            (proc->extension_enabled(EXT_ZICFILP) ? HENVCFG_LPE : 0) |
                            (proc->extension_enabled(EXT_ZICFISS) ? HENVCFG_SSE : 0) |
                            (proc->extension_enabled(EXT_SSDBLTRP) ? HENVCFG_DTE : 0);
  henvcfg = std::make_shared<henvcfg_csr_t>(proc, CSR_HENVCFG, henvcfg_mask, 0, menvcfg);
  if (xlen == 32) {
    add_hypervisor_csr(CSR_HENVCFG, std::make_shared<rv32_low_csr_t>(proc, CSR_HENVCFG, henvcfg));
    add_hypervisor_csr(CSR_HENVCFGH, std::make_shared<rv32_high_csr_t>(proc, CSR_HENVCFGH, henvcfg));
  } else {
    add_hypervisor_csr(CSR_HENVCFG, henvcfg);
  }
  if (proc->extension_enabled_const(EXT_SMSTATEEN)) {
    const reg_t sstateen0_mask = (proc->extension_enabled(EXT_ZFINX) ? SSTATEEN0_FCSR : 0) |
                                 (proc->extension_enabled(EXT_ZCMT) ? SSTATEEN0_JVT : 0) |
                                 SSTATEEN0_CS;
    const reg_t hstateen0_mask = sstateen0_mask | HSTATEEN0_SENVCFG | HSTATEEN_SSTATEEN;
    const reg_t mstateen0_mask = hstateen0_mask | (proc->extension_enabled(EXT_SSQOSID) ?  MSTATEEN0_PRIV114 : 0);
    for (int i = 0; i < 4; i++) {
      const reg_t mstateen_mask = i == 0 ? mstateen0_mask : MSTATEEN_HSTATEEN;
      mstateen[i] = std::make_shared<masked_csr_t>(proc, CSR_MSTATEEN0 + i, mstateen_mask, 0);
      if (xlen == 32) {
        add_csr(CSR_MSTATEEN0 + i, std::make_shared<rv32_low_csr_t>(proc, CSR_MSTATEEN0 + i, mstateen[i]));
        add_csr(CSR_MSTATEEN0H + i, std::make_shared<rv32_high_csr_t>(proc, CSR_MSTATEEN0H + i, mstateen[i]));
      } else {
        add_csr(CSR_MSTATEEN0 + i, mstateen[i]);
      }

      const reg_t hstateen_mask = i == 0 ? hstateen0_mask : HSTATEEN_SSTATEEN;
      hstateen[i] = std::make_shared<hstateen_csr_t>(proc, CSR_HSTATEEN0 + i, hstateen_mask, 0, i);
      if (xlen == 32) {
        add_hypervisor_csr(CSR_HSTATEEN0 + i, std::make_shared<rv32_low_csr_t>(proc, CSR_HSTATEEN0 + i, hstateen[i]));
        add_hypervisor_csr(CSR_HSTATEEN0H + i, std::make_shared<rv32_high_csr_t>(proc, CSR_HSTATEEN0H + i, hstateen[i]));
      } else {
        add_hypervisor_csr(CSR_HSTATEEN0 + i, hstateen[i]);
      }

      const reg_t sstateen_mask = i == 0 ? sstateen0_mask : 0;
      add_supervisor_csr(CSR_SSTATEEN0 + i, sstateen[i] = std::make_shared<sstateen_csr_t>(proc, CSR_SSTATEEN0 + i, sstateen_mask, 0, i));
    }
  }

  if (proc->extension_enabled_const(EXT_SMRNMI)) {
    add_csr(CSR_MNSCRATCH, std::make_shared<basic_csr_t>(proc, CSR_MNSCRATCH, 0));
    add_csr(CSR_MNEPC, mnepc = std::make_shared<epc_csr_t>(proc, CSR_MNEPC));
    add_csr(CSR_MNCAUSE, std::make_shared<const_csr_t>(proc, CSR_MNCAUSE, (reg_t)1 << (xlen - 1)));
    add_csr(CSR_MNSTATUS, mnstatus = std::make_shared<mnstatus_csr_t>(proc, CSR_MNSTATUS));
  }

  if (proc->extension_enabled_const(EXT_SSTC)) {
    stimecmp = std::make_shared<stimecmp_csr_t>(proc, CSR_STIMECMP, MIP_STIP);
    vstimecmp = std::make_shared<stimecmp_csr_t>(proc, CSR_VSTIMECMP, MIP_VSTIP);
    auto virtualized_stimecmp = std::make_shared<virtualized_stimecmp_csr_t>(proc, stimecmp, vstimecmp);
    if (xlen == 32) {
      add_supervisor_csr(CSR_STIMECMP, std::make_shared<rv32_low_csr_t>(proc, CSR_STIMECMP, virtualized_stimecmp));
      add_supervisor_csr(CSR_STIMECMPH, std::make_shared<rv32_high_csr_t>(proc, CSR_STIMECMPH, virtualized_stimecmp));
      add_hypervisor_csr(CSR_VSTIMECMP, std::make_shared<rv32_low_csr_t>(proc, CSR_VSTIMECMP, vstimecmp));
      add_hypervisor_csr(CSR_VSTIMECMPH, std::make_shared<rv32_high_csr_t>(proc, CSR_VSTIMECMPH, vstimecmp));
    } else {
      add_supervisor_csr(CSR_STIMECMP, virtualized_stimecmp);
      add_hypervisor_csr(CSR_VSTIMECMP, vstimecmp);
    }
  }

  add_ext_csr(EXT_ZCMT, CSR_JVT, jvt = std::make_shared<jvt_csr_t>(proc, CSR_JVT, 0));

  const reg_t ssp_mask = -reg_t(xlen / 8);
  add_ext_csr(EXT_ZICFISS, CSR_SSP, ssp = std::make_shared<ssp_csr_t>(proc, CSR_SSP, ssp_mask, 0));


  // Smcsrind / Sscsrind
  if (proc->extension_enabled_const(EXT_SMCSRIND)) {
    csr_t_p miselect = std::make_shared<basic_csr_t>(proc, CSR_MISELECT, 0);
    add_csr(CSR_MISELECT, miselect);

    const reg_t mireg_csrs[] = { CSR_MIREG, CSR_MIREG2, CSR_MIREG3, CSR_MIREG4, CSR_MIREG5, CSR_MIREG6 };
    for (auto csr : mireg_csrs)
      add_csr(csr, std::make_shared<sscsrind_reg_csr_t>(proc, csr, miselect));
  }

  if (proc->extension_enabled_const(EXT_SSCSRIND)) {
    csr_t_p vsiselect = std::make_shared<basic_csr_t>(proc, CSR_VSISELECT, 0);
    add_hypervisor_csr(CSR_VSISELECT, vsiselect);

    csr_t_p siselect = std::make_shared<basic_csr_t>(proc, CSR_SISELECT, 0);
    add_supervisor_csr(CSR_SISELECT, std::make_shared<virtualized_csr_t>(proc, siselect, vsiselect));

    const reg_t vsireg_csrs[] = { CSR_VSIREG, CSR_VSIREG2, CSR_VSIREG3, CSR_VSIREG4, CSR_VSIREG5, CSR_VSIREG6 };
    const reg_t sireg_csrs[] = { CSR_SIREG, CSR_SIREG2, CSR_SIREG3, CSR_SIREG4, CSR_SIREG5, CSR_SIREG6 };
    for (size_t i = 0; i < std::size(vsireg_csrs); i++) {
      auto vsireg = std::make_shared<sscsrind_reg_csr_t>(proc, vsireg_csrs[i], vsiselect);
      add_hypervisor_csr(vsireg_csrs[i], vsireg);

      auto sireg = std::make_shared<sscsrind_reg_csr_t>(proc, sireg_csrs[i], siselect);
      add_supervisor_csr(sireg_csrs[i], std::make_shared<virtualized_indirect_csr_t>(proc, sireg, vsireg));
    }
  }

  if (proc->extension_enabled_const(EXT_SMCNTRPMF)) {
    if (xlen == 32) {
      add_csr(CSR_MCYCLECFG, std::make_shared<rv32_low_csr_t>(proc, CSR_MCYCLECFG, mcyclecfg));
      add_csr(CSR_MCYCLECFGH, std::make_shared<rv32_high_csr_t>(proc, CSR_MCYCLECFGH, mcyclecfg));
      add_csr(CSR_MINSTRETCFG, std::make_shared<rv32_low_csr_t>(proc, CSR_MINSTRETCFG, minstretcfg));
      add_csr(CSR_MINSTRETCFGH, std::make_shared<rv32_high_csr_t>(proc, CSR_MINSTRETCFGH, minstretcfg));
    } else {
      add_csr(CSR_MCYCLECFG, mcyclecfg);
      add_csr(CSR_MINSTRETCFG, minstretcfg);
    }
  }

  const reg_t srmcfg_mask = SRMCFG_MCID | SRMCFG_RCID;
  add_const_ext_csr(EXT_SSQOSID, CSR_SRMCFG, std::make_shared<srmcfg_csr_t>(proc, CSR_SRMCFG, srmcfg_mask, 0));
}
