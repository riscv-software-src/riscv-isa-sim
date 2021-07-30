// See LICENSE for license details.

#include "arith.h"
#include "processor.h"
#include "extension.h"
#include "common.h"
#include "config.h"
#include "simif.h"
#include "mmu.h"
#include "disasm.h"
#include "platform.h"
#include <cinttypes>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <limits.h>
#include <stdexcept>
#include <string>
#include <algorithm>

#undef STATE
#define STATE state

processor_t::processor_t(const char* isa, const char* priv, const char* varch,
                         simif_t* sim, uint32_t id, bool halt_on_reset,
                         FILE* log_file)
  : debug(false), halt_request(HR_NONE), sim(sim), id(id), xlen(0),
  histogram_enabled(false), log_commits_enabled(false),
  log_file(log_file), halt_on_reset(halt_on_reset),
  extension_table(256, false), impl_table(256, false), last_pc(1), executions(1)
{
  VU.p = this;

  parse_isa_string(isa);
  parse_priv_string(priv);
  parse_varch_string(varch);

  register_base_instructions();
  mmu = new mmu_t(sim, this);

  disassembler = new disassembler_t(max_xlen);
  for (auto e : custom_extensions)
    for (auto disasm_insn : e.second->get_disasms())
      disassembler->add_insn(disasm_insn);

  set_pmp_granularity(1 << PMP_SHIFT);
  set_pmp_num(state.max_pmp);

  if (max_xlen == 32)
    set_mmu_capability(IMPL_MMU_SV32);
  else if (max_xlen == 64)
    set_mmu_capability(IMPL_MMU_SV48);

  reset();
}

processor_t::~processor_t()
{
#ifdef RISCV_ENABLE_HISTOGRAM
  if (histogram_enabled)
  {
    fprintf(stderr, "PC Histogram size:%zu\n", pc_histogram.size());
    for (auto it : pc_histogram)
      fprintf(stderr, "%0" PRIx64 " %" PRIu64 "\n", it.first, it.second);
  }
#endif

  delete mmu;
  delete disassembler;
}

static void bad_option_string(const char *option, const char *value,
                              const char *msg)
{
  fprintf(stderr, "error: bad %s option '%s'. %s\n", option, value, msg);
  abort();
}

static void bad_isa_string(const char* isa, const char* msg)
{
  bad_option_string("--isa", isa, msg);
}

static void bad_priv_string(const char* priv)
{
  fprintf(stderr, "error: bad --priv option %s\n", priv);
  abort();
}

static void bad_varch_string(const char* varch, const char *msg)
{
  bad_option_string("--varch", varch, msg);
}

static std::string get_string_token(std::string str, const char delimiter, size_t& pos)
{
  size_t _pos = pos;
  while (pos < str.length() && str[pos] != delimiter) ++pos;
  return str.substr(_pos, pos - _pos);
}

static int get_int_token(std::string str, const char delimiter, size_t& pos)
{
  size_t _pos = pos;
  while (pos < str.length() && str[pos] != delimiter) {
    if (!isdigit(str[pos]))
      bad_varch_string(str.c_str(), "Unsupported value"); // An integer is expected
    ++pos;
  }
  return (pos == _pos) ? 0 : stoi(str.substr(_pos, pos - _pos));
}

static bool check_pow2(int val)
{
  return ((val & (val - 1))) == 0;
}

void processor_t::parse_varch_string(const char* s)
{
  std::string str, tmp;
  for (const char *r = s; *r; r++)
    str += std::tolower(*r);

  size_t pos = 0;
  size_t len = str.length();
  int vlen = 0;
  int elen = 0;
  int slen = 0;
  int vstart_alu = 1;

  while (pos < len) {
    std::string attr = get_string_token(str, ':', pos);

    ++pos;

    if (attr == "vlen")
      vlen = get_int_token(str, ',', pos);
    else if (attr == "slen")
      slen = get_int_token(str, ',', pos);
    else if (attr == "elen")
      elen = get_int_token(str, ',', pos);
    else if (attr == "vstartalu")
      vstart_alu = get_int_token(str, ',', pos);
    else
      bad_varch_string(s, "Unsupported token");

    ++pos;
  }

  // The integer should be the power of 2
  if (!check_pow2(vlen) || !check_pow2(elen) || !check_pow2(slen)){
    bad_varch_string(s, "The integer value should be the power of 2");
  }

  if (slen == 0)
    slen = vlen;

  /* Vector spec requirements. */
  if (vlen < elen)
    bad_varch_string(s, "vlen must be >= elen");
  if (vlen != slen)
    bad_varch_string(s, "vlen must be == slen for current limitation");

  /* spike requirements. */
  if (vlen > 4096)
    bad_varch_string(s, "vlen must be <= 4096");

  VU.VLEN = vlen;
  VU.ELEN = elen;
  VU.vlenb = vlen / 8;
  VU.vstart_alu = vstart_alu;
}

static std::string strtolower(const char* str)
{
  std::string res;
  for (const char *r = str; *r; r++)
    res += std::tolower(*r);
  return res;
}

void processor_t::parse_priv_string(const char* str)
{
  std::string lowercase = strtolower(str);
  bool user = false, supervisor = false;

  if (lowercase == "m")
    ;
  else if (lowercase == "mu")
    user = true;
  else if (lowercase == "msu")
    user = supervisor = true;
  else
    bad_priv_string(str);

  if (user) {
    max_isa |= reg_t(user) << ('u' - 'a');
    extension_table['U'] = true;
  }

  if (supervisor) {
    max_isa |= reg_t(supervisor) << ('s' - 'a');
    extension_table['S'] = true;
  }
}

void processor_t::parse_isa_string(const char* str)
{
  std::string lowercase = strtolower(str), tmp;

  char error_msg[256];
  const char* p = lowercase.c_str();
  const char* all_subsets = "imafdqcbkhp"
#ifdef __SIZEOF_INT128__
    "v"
#endif
    "";

  max_xlen = 64;
  max_isa = reg_t(2) << 62;

  if (strncmp(p, "rv32", 4) == 0)
    max_xlen = 32, max_isa = reg_t(1) << 30, p += 4;
  else if (strncmp(p, "rv64", 4) == 0)
    p += 4;
  else if (strncmp(p, "rv", 2) == 0)
    p += 2;

  if (!*p) {
    p = "imafdc";
  } else if (*p == 'g') { // treat "G" as "IMAFD"
    tmp = std::string("imafd") + (p+1);
    p = &tmp[0];
  }

  isa_string = "rv" + std::to_string(max_xlen) + p;

  while (*p) {
    if (islower(*p)) {
      max_isa |= 1L << (*p - 'a');
      extension_table[toupper(*p)] = true;

      if (strchr(all_subsets, *p)) {
        p++;
      } else if (*p == 'x') {
        const char* ext = p + 1, *end = ext;
        while (islower(*end))
          end++;

        auto ext_str = std::string(ext, end - ext);
        if (ext_str != "dummy")
          register_extension(find_extension(ext_str.c_str())());

        p = end;
      } else {
        sprintf(error_msg, "unsupported extension '%c'", *p);
        bad_isa_string(str, error_msg);
      }
    } else if (*p == '_') {
      const char* ext = p + 1, *end = ext;
      if (*ext == 'x') {
        p++;
        continue;
      }

      while (islower(*end))
        end++;

      auto ext_str = std::string(ext, end - ext);
      if (ext_str == "zfh") {
        extension_table[EXT_ZFH] = true;
      } else if (ext_str == "zba") {
        extension_table[EXT_ZBA] = true;
      } else if (ext_str == "zbb") {
        extension_table[EXT_ZBB] = true;
      } else if (ext_str == "zbc") {
        extension_table[EXT_ZBC] = true;
      } else if (ext_str == "zbs") {
        extension_table[EXT_ZBS] = true;
      } else {
        sprintf(error_msg, "unsupported extension '%s'", ext_str.c_str());
        bad_isa_string(str, error_msg);
      }

      p = end;
    } else {
      sprintf(error_msg, "can't parse '%c(%d)'", *p, *p);
      bad_isa_string(str, error_msg);
    }
  }

  state.misa = max_isa;

  if (supports_extension('B')) {
    // B implies Zba, Zbb, Zbc, Zbs
    extension_table[EXT_ZBA] = true;
    extension_table[EXT_ZBB] = true;
    extension_table[EXT_ZBC] = true;
    extension_table[EXT_ZBS] = true;
  }

  if (!supports_extension('I'))
    bad_isa_string(str, "'I' extension is required");

  if (supports_extension(EXT_ZFH) && !supports_extension('F'))
    bad_isa_string(str, "'Zfh' extension requires 'F'");

  if (supports_extension('D') && !supports_extension('F'))
    bad_isa_string(str, "'D' extension requires 'F'");

  if (supports_extension('Q') && !supports_extension('D'))
    bad_isa_string(str, "'Q' extension requires 'D'");
}

void state_t::reset(reg_t max_isa)
{
  pc = DEFAULT_RSTVEC;
  XPR.reset();
  FPR.reset();

  prv = PRV_M;
  v = false;
  misa = max_isa;
  mstatus = 0;
  mepc = 0;
  mtval = 0;
  mscratch = 0;
  mtvec = 0;
  mcause = 0;
  minstret = 0;
  mie = 0;
  mip = 0;
  medeleg = 0;
  mideleg = 0;
  mcounteren = 0;
  scounteren = 0;
  sepc = 0;
  stval = 0;
  sscratch = 0;
  stvec = 0;
  satp = 0;
  scause = 0;
  mtval2 = 0;
  mtinst = 0;
  hstatus = 0;
  hideleg = 0;
  hedeleg = 0;
  hcounteren = 0;
  htval = 0;
  htinst = 0;
  hgatp = 0;
  vsstatus = 0;
  vstvec = 0;
  vsscratch = 0;
  vsepc = 0;
  vscause = 0;
  vstval = 0;
  vsatp = 0;

  dpc = 0;
  dscratch0 = 0;
  dscratch1 = 0;
  memset(&this->dcsr, 0, sizeof(this->dcsr));

  tselect = 0;
  memset(this->mcontrol, 0, sizeof(this->mcontrol));
  for (auto &item : mcontrol)
    item.type = 2;

  memset(this->tdata2, 0, sizeof(this->tdata2));
  debug_mode = false;
  single_step = STEP_NONE;

  memset(this->pmpcfg, 0, sizeof(this->pmpcfg));
  memset(this->pmpaddr, 0, sizeof(this->pmpaddr));

  fflags = 0;
  frm = 0;
  serialized = false;

#ifdef RISCV_ENABLE_COMMITLOG
  log_reg_write.clear();
  log_mem_read.clear();
  log_mem_write.clear();
  last_inst_priv = 0;
  last_inst_xlen = 0;
  last_inst_flen = 0;
#endif
}

void processor_t::vectorUnit_t::reset(){
  free(reg_file);
  VLEN = get_vlen();
  ELEN = get_elen();
  reg_file = malloc(NVPR * vlenb);
  memset(reg_file, 0, NVPR * vlenb);

  vtype = 0;
  set_vl(0, 0, 0, -1); // default to illegal configuration
}

reg_t processor_t::vectorUnit_t::set_vl(int rd, int rs1, reg_t reqVL, reg_t newType){
  int new_vlmul = 0;
  if (vtype != newType){
    vtype = newType;
    vsew = 1 << (extract64(newType, 3, 3) + 3);
    new_vlmul = int8_t(extract64(newType, 0, 3) << 5) >> 5;
    vflmul = new_vlmul >= 0 ? 1 << new_vlmul : 1.0 / (1 << -new_vlmul);
    vlmax = (VLEN/vsew) * vflmul;
    vta = extract64(newType, 6, 1);
    vma = extract64(newType, 7, 1);

    vill = !(vflmul >= 0.125 && vflmul <= 8)
           || vsew > std::min(vflmul, 1.0f) * ELEN
           || (newType >> 8) != 0;

    if (vill) {
      vlmax = 0;
      vtype = UINT64_MAX << (p->get_xlen() - 1);
    }
  }

  // set vl
  if (vlmax == 0) {
    vl = 0;
  } else if (rd == 0 && rs1 == 0) {
    vl = vl > vlmax ? vlmax : vl;
  } else if (rd != 0 && rs1 == 0) {
    vl = vlmax;
  } else if (rs1 != 0) {
    vl = reqVL > vlmax ? vlmax : reqVL;
  }

  vstart = 0;
  setvl_count++;
  return vl;
}

void processor_t::set_debug(bool value)
{
  debug = value;

  for (auto e : custom_extensions)
    e.second->set_debug(value);
}

void processor_t::set_histogram(bool value)
{
  histogram_enabled = value;
#ifndef RISCV_ENABLE_HISTOGRAM
  if (value) {
    fprintf(stderr, "PC Histogram support has not been properly enabled;");
    fprintf(stderr, " please re-build the riscv-isa-sim project using \"configure --enable-histogram\".\n");
    abort();
  }
#endif
}

#ifdef RISCV_ENABLE_COMMITLOG
void processor_t::enable_log_commits()
{
  log_commits_enabled = true;
}
#endif

void processor_t::reset()
{
  state.reset(max_isa);
#ifdef RISCV_ENABLE_DUAL_ENDIAN
  if (mmu->is_target_big_endian())
    state.mstatus |= MSTATUS_UBE | MSTATUS_SBE | MSTATUS_MBE;
#endif

  state.mideleg = supports_extension('H') ? MIDELEG_FORCED_MASK : 0;

  state.dcsr.halt = halt_on_reset;
  halt_on_reset = false;
  set_csr(CSR_MSTATUS, state.mstatus);
  state.vsstatus = state.mstatus & SSTATUS_VS_MASK;  // set UXL
  set_csr(CSR_HSTATUS, state.hstatus);  // set VSXL
  VU.reset();

  if (n_pmp > 0) {
    // For backwards compatibility with software that is unaware of PMP,
    // initialize PMP to permit unprivileged access to all of memory.
    set_csr(CSR_PMPADDR0, ~reg_t(0));
    set_csr(CSR_PMPCFG0, PMP_R | PMP_W | PMP_X | PMP_NAPOT);
  }

   for (auto e : custom_extensions) // reset any extensions
    e.second->reset();

  if (sim)
    sim->proc_reset(id);
}

extension_t* processor_t::get_extension()
{
  switch (custom_extensions.size()) {
    case 0: return NULL;
    case 1: return custom_extensions.begin()->second;
    default:
      fprintf(stderr, "processor_t::get_extension() is ambiguous when multiple extensions\n");
      fprintf(stderr, "are present!\n");
      abort();
  }
}

extension_t* processor_t::get_extension(const char* name)
{
  auto it = custom_extensions.find(name);
  if (it == custom_extensions.end())
    abort();
  return it->second;
}

void processor_t::set_pmp_num(reg_t n)
{
  // check the number of pmp is in a reasonable range
  if (n > state.max_pmp) {
    fprintf(stderr, "error: bad number of pmp regions: '%ld' from the dtb\n", (unsigned long)n);
    abort();
  }
  n_pmp = n;
}

void processor_t::set_pmp_granularity(reg_t gran) {
  // check the pmp granularity is set from dtb(!=0) and is power of 2
  if (gran < (1 << PMP_SHIFT) || (gran & (gran - 1)) != 0) {
    fprintf(stderr, "error: bad pmp granularity '%ld' from the dtb\n", (unsigned long)gran);
    abort();
  }

  lg_pmp_granularity = ctz(gran);
}

void processor_t::set_mmu_capability(int cap)
{
  switch (cap) {
    case IMPL_MMU_SV32:
      set_impl(cap, true);
      set_impl(IMPL_MMU, true);
      break;
    case IMPL_MMU_SV39:
      set_impl(cap, true);
      set_impl(IMPL_MMU, true);
      break;
    case IMPL_MMU_SV48:
      set_impl(cap, true);
      set_impl(IMPL_MMU_SV39, true);
      set_impl(IMPL_MMU, true);
      break;
    default:
      set_impl(IMPL_MMU_SV32, false);
      set_impl(IMPL_MMU_SV39, false);
      set_impl(IMPL_MMU_SV48, false);
      set_impl(IMPL_MMU, false);
      break;
  }
}

void processor_t::take_interrupt(reg_t pending_interrupts)
{
  reg_t enabled_interrupts, deleg, status, mie, m_enabled;
  reg_t hsie, hs_enabled, vsie, vs_enabled;

  // Do nothing if no pending interrupts
  if (!pending_interrupts) {
    return;
  }

  // M-ints have higher priority over HS-ints and VS-ints
  mie = get_field(state.mstatus, MSTATUS_MIE);
  m_enabled = state.prv < PRV_M || (state.prv == PRV_M && mie);
  enabled_interrupts = pending_interrupts & ~state.mideleg & -m_enabled;
  if (enabled_interrupts == 0) {
    // HS-ints have higher priority over VS-ints
    deleg = state.mideleg & ~state.hideleg;
    status = (state.v) ? state.vsstatus : state.mstatus;
    hsie = get_field(status, MSTATUS_SIE);
    hs_enabled = state.v || state.prv < PRV_S || (state.prv == PRV_S && hsie);
    enabled_interrupts = pending_interrupts & deleg & -hs_enabled;
    if (state.v && enabled_interrupts == 0) {
      // VS-ints have least priority and can only be taken with virt enabled
      deleg = state.mideleg & state.hideleg;
      vsie = get_field(state.mstatus, MSTATUS_SIE);
      vs_enabled = state.prv < PRV_S || (state.prv == PRV_S && vsie);
      enabled_interrupts = pending_interrupts & deleg & -vs_enabled;
    }
  }

  if (!state.debug_mode && enabled_interrupts) {
    // nonstandard interrupts have highest priority
    if (enabled_interrupts >> IRQ_M_EXT)
      enabled_interrupts = enabled_interrupts >> IRQ_M_EXT << IRQ_M_EXT;
    // standard interrupt priority is MEI, MSI, MTI, SEI, SSI, STI
    else if (enabled_interrupts & MIP_MEIP)
      enabled_interrupts = MIP_MEIP;
    else if (enabled_interrupts & MIP_MSIP)
      enabled_interrupts = MIP_MSIP;
    else if (enabled_interrupts & MIP_MTIP)
      enabled_interrupts = MIP_MTIP;
    else if (enabled_interrupts & MIP_SEIP)
      enabled_interrupts = MIP_SEIP;
    else if (enabled_interrupts & MIP_SSIP)
      enabled_interrupts = MIP_SSIP;
    else if (enabled_interrupts & MIP_STIP)
      enabled_interrupts = MIP_STIP;
    else if (enabled_interrupts & MIP_VSEIP)
      enabled_interrupts = MIP_VSEIP;
    else if (enabled_interrupts & MIP_VSSIP)
      enabled_interrupts = MIP_VSSIP;
    else if (enabled_interrupts & MIP_VSTIP)
      enabled_interrupts = MIP_VSTIP;
    else
      abort();

    throw trap_t(((reg_t)1 << (max_xlen-1)) | ctz(enabled_interrupts));
  }
}

static int xlen_to_uxl(int xlen)
{
  if (xlen == 32)
    return 1;
  if (xlen == 64)
    return 2;
  abort();
}

reg_t processor_t::legalize_privilege(reg_t prv)
{
  assert(prv <= PRV_M);

  if (!supports_extension('U'))
    return PRV_M;

  if (prv == PRV_HS || (prv == PRV_S && !supports_extension('S')))
    return PRV_U;

  return prv;
}

void processor_t::set_privilege(reg_t prv)
{
  mmu->flush_tlb();
  state.prv = legalize_privilege(prv);
}

void processor_t::set_virt(bool virt)
{
  reg_t tmp, mask;

  if (state.prv == PRV_M)
    return;

  if (state.v != virt) {
    /*
     * Ideally, we should flush TLB here but we don't need it because
     * set_virt() is always used in conjucter with set_privilege() and
     * set_privilege() will flush TLB unconditionally.
     */
    mask = SSTATUS_VS_MASK;
    mask |= (supports_extension('V') ? SSTATUS_VS : 0);
    mask |= (xlen == 64 ? SSTATUS64_SD : SSTATUS32_SD);
    tmp = state.mstatus & mask;
    state.mstatus = (state.mstatus & ~mask) | (state.vsstatus & mask);
    state.vsstatus = tmp;
    state.v = virt;
  }
}

void processor_t::enter_debug_mode(uint8_t cause)
{
  state.debug_mode = true;
  state.dcsr.cause = cause;
  state.dcsr.prv = state.prv;
  set_privilege(PRV_M);
  state.dpc = state.pc;
  state.pc = DEBUG_ROM_ENTRY;
}

void processor_t::take_trap(trap_t& t, reg_t epc)
{
  if (debug) {
    fprintf(log_file, "core %3d: exception %s, epc 0x%0*" PRIx64 "\n",
            id, t.name(), max_xlen/4, zext(epc, max_xlen));
    if (t.has_tval())
      fprintf(log_file, "core %3d:           tval 0x%0*" PRIx64 "\n",
              id, max_xlen/4, zext(t.get_tval(), max_xlen));
  }

  if (state.debug_mode) {
    if (t.cause() == CAUSE_BREAKPOINT) {
      state.pc = DEBUG_ROM_ENTRY;
    } else {
      state.pc = DEBUG_ROM_TVEC;
    }
    return;
  }

  if (t.cause() == CAUSE_BREAKPOINT && (
              (state.prv == PRV_M && state.dcsr.ebreakm) ||
              (state.prv == PRV_S && state.dcsr.ebreaks) ||
              (state.prv == PRV_U && state.dcsr.ebreaku))) {
    enter_debug_mode(DCSR_CAUSE_SWBP);
    return;
  }

  // By default, trap to M-mode, unless delegated to HS-mode or VS-mode
  reg_t vsdeleg, hsdeleg;
  reg_t bit = t.cause();
  bool curr_virt = state.v;
  bool interrupt = (bit & ((reg_t)1 << (max_xlen-1))) != 0;
  if (interrupt) {
    vsdeleg = (curr_virt && state.prv <= PRV_S) ? (state.mideleg & state.hideleg) : 0;
    hsdeleg = (state.prv <= PRV_S) ? state.mideleg : 0;
    bit &= ~((reg_t)1 << (max_xlen-1));
  } else {
    vsdeleg = (curr_virt && state.prv <= PRV_S) ? (state.medeleg & state.hedeleg) : 0;
    hsdeleg = (state.prv <= PRV_S) ? state.medeleg : 0;
  }
  if (state.prv <= PRV_S && bit < max_xlen && ((vsdeleg >> bit) & 1)) {
    // Handle the trap in VS-mode
    reg_t vector = (state.vstvec & 1) && interrupt ? 4*bit : 0;
    state.pc = (state.vstvec & ~(reg_t)1) + vector;
    state.vscause = (interrupt) ? (t.cause() - 1) : t.cause();
    state.vsepc = epc;
    state.vstval = t.get_tval();

    reg_t s = state.mstatus;
    s = set_field(s, MSTATUS_SPIE, get_field(s, MSTATUS_SIE));
    s = set_field(s, MSTATUS_SPP, state.prv);
    s = set_field(s, MSTATUS_SIE, 0);
    set_csr(CSR_MSTATUS, s);
    set_privilege(PRV_S);
  } else if (state.prv <= PRV_S && bit < max_xlen && ((hsdeleg >> bit) & 1)) {
    // Handle the trap in HS-mode
    set_virt(false);
    reg_t vector = (state.stvec & 1) && interrupt ? 4*bit : 0;
    state.pc = (state.stvec & ~(reg_t)1) + vector;
    state.scause = t.cause();
    state.sepc = epc;
    state.stval = t.get_tval();
    state.htval = t.get_tval2();
    state.htinst = t.get_tinst();

    reg_t s = state.mstatus;
    s = set_field(s, MSTATUS_SPIE, get_field(s, MSTATUS_SIE));
    s = set_field(s, MSTATUS_SPP, state.prv);
    s = set_field(s, MSTATUS_SIE, 0);
    set_csr(CSR_MSTATUS, s);
    s = state.hstatus;
    if (curr_virt)
      s = set_field(s, HSTATUS_SPVP, state.prv);
    s = set_field(s, HSTATUS_SPV, curr_virt);
    s = set_field(s, HSTATUS_GVA, t.has_gva());
    set_csr(CSR_HSTATUS, s);
    set_privilege(PRV_S);
  } else {
    // Handle the trap in M-mode
    set_virt(false);
    reg_t vector = (state.mtvec & 1) && interrupt ? 4*bit : 0;
    state.pc = (state.mtvec & ~(reg_t)1) + vector;
    state.mepc = epc;
    state.mcause = t.cause();
    state.mtval = t.get_tval();
    state.mtval2 = t.get_tval2();
    state.mtinst = t.get_tinst();

    reg_t s = state.mstatus;
    s = set_field(s, MSTATUS_MPIE, get_field(s, MSTATUS_MIE));
    s = set_field(s, MSTATUS_MPP, state.prv);
    s = set_field(s, MSTATUS_MIE, 0);
    s = set_field(s, MSTATUS_MPV, curr_virt);
    s = set_field(s, MSTATUS_GVA, t.has_gva());
    set_csr(CSR_MSTATUS, s);
    set_privilege(PRV_M);
  }
}

void processor_t::disasm(insn_t insn)
{
  uint64_t bits = insn.bits() & ((1ULL << (8 * insn_length(insn.bits()))) - 1);
  if (last_pc != state.pc || last_bits != bits) {

#ifdef RISCV_ENABLE_COMMITLOG
    const char* sym = get_symbol(state.pc);
    if (sym != nullptr)
    {
      fprintf(log_file, "core %3d: >>>>  %s\n", id, sym);
    }
#endif

    if (executions != 1) {
      fprintf(log_file, "core %3d: Executed %" PRIx64 " times\n", id, executions);
    }

    fprintf(log_file, "core %3d: 0x%0*" PRIx64 " (0x%08" PRIx64 ") %s\n",
            id, max_xlen/4, zext(state.pc, max_xlen), bits,
            disassembler->disassemble(insn).c_str());
    last_pc = state.pc;
    last_bits = bits;
    executions = 1;
  } else {
    executions++;
  }
}

int processor_t::paddr_bits()
{
  assert(xlen == max_xlen);
  return max_xlen == 64 ? 50 : 34;
}

bool processor_t::satp_valid(reg_t val) const
{
  if (xlen == 32) {
    switch (get_field(val, SATP32_MODE)) {
      case SATP_MODE_SV32: return supports_impl(IMPL_MMU_SV32);
      case SATP_MODE_OFF: return true;
      default: return false;
    }
  } else {
    switch (get_field(val, SATP64_MODE)) {
      case SATP_MODE_SV39: return supports_impl(IMPL_MMU_SV39);
      case SATP_MODE_SV48: return supports_impl(IMPL_MMU_SV48);
      case SATP_MODE_OFF: return true;
      default: return false;
    }
  }
}

reg_t processor_t::compute_new_satp(reg_t val, reg_t old) const
{
  reg_t rv64_ppn_mask = (reg_t(1) << (MAX_PADDR_BITS - PGSHIFT)) - 1;

  reg_t mode_mask = xlen == 32 ? SATP32_MODE : SATP64_MODE;
  reg_t ppn_mask = xlen == 32 ? SATP32_PPN : SATP64_PPN & rv64_ppn_mask;
  reg_t new_mask = (satp_valid(val) ? mode_mask : 0) | ppn_mask;
  reg_t old_mask = satp_valid(val) ? 0 : mode_mask;

  return (new_mask & val) | (old_mask & old);
}

void processor_t::set_csr(int which, reg_t val)
{
#if defined(RISCV_ENABLE_COMMITLOG)
#define LOG_CSR(rd) \
  STATE.log_reg_write[((which) << 4) | 4] = {get_csr(rd), 0};
#else
#define LOG_CSR(rd)
#endif

  val = zext_xlen(val);
  reg_t supervisor_ints = supports_extension('S') ? MIP_SSIP | MIP_STIP | MIP_SEIP : 0;
  reg_t vssip_int = supports_extension('H') ? MIP_VSSIP : 0;
  reg_t hypervisor_ints = supports_extension('H') ? MIP_HS_MASK : 0;
  reg_t coprocessor_ints = (!custom_extensions.empty()) << IRQ_COP;
  reg_t delegable_ints = supervisor_ints | coprocessor_ints;
  reg_t all_ints = delegable_ints | hypervisor_ints | MIP_MSIP | MIP_MTIP | MIP_MEIP;
  reg_t hypervisor_exceptions = 0
    | (1 << CAUSE_VIRTUAL_SUPERVISOR_ECALL)
    | (1 << CAUSE_FETCH_GUEST_PAGE_FAULT)
    | (1 << CAUSE_LOAD_GUEST_PAGE_FAULT)
    | (1 << CAUSE_VIRTUAL_INSTRUCTION)
    | (1 << CAUSE_STORE_GUEST_PAGE_FAULT)
    ;

  if (which >= CSR_PMPADDR0 && which < CSR_PMPADDR0 + state.max_pmp) {
    // If no PMPs are configured, disallow access to all.  Otherwise, allow
    // access to all, but unimplemented ones are hardwired to zero.
    if (n_pmp == 0)
      return;

    size_t i = which - CSR_PMPADDR0;
    bool locked = state.pmpcfg[i] & PMP_L;
    bool next_locked = i+1 < state.max_pmp && (state.pmpcfg[i+1] & PMP_L);
    bool next_tor = i+1 < state.max_pmp && (state.pmpcfg[i+1] & PMP_A) == PMP_TOR;
    if (i < n_pmp && !locked && !(next_locked && next_tor)) {
      state.pmpaddr[i] = val & ((reg_t(1) << (MAX_PADDR_BITS - PMP_SHIFT)) - 1);
      LOG_CSR(which);
    }

    mmu->flush_tlb();
  }

  if (which >= CSR_PMPCFG0 && which < CSR_PMPCFG0 + state.max_pmp / 4) {
    if (n_pmp == 0)
      return;

    for (size_t i0 = (which - CSR_PMPCFG0) * 4, i = i0; i < i0 + xlen / 8; i++) {
      if (i < n_pmp && !(state.pmpcfg[i] & PMP_L)) {
        uint8_t cfg = (val >> (8 * (i - i0))) & (PMP_R | PMP_W | PMP_X | PMP_A | PMP_L);
        cfg &= ~PMP_W | ((cfg & PMP_R) ? PMP_W : 0); // Disallow R=0 W=1
        if (lg_pmp_granularity != PMP_SHIFT && (cfg & PMP_A) == PMP_NA4)
          cfg |= PMP_NAPOT; // Disallow A=NA4 when granularity > 4
        state.pmpcfg[i] = cfg;
        LOG_CSR(which);
      }
    }
    mmu->flush_tlb();
  }

  switch (which)
  {
    case CSR_SENTROPY:
      es.set_sentropy(val);
      break;
    case CSR_FFLAGS:
      dirty_fp_state;
      state.fflags = val & (FSR_AEXC >> FSR_AEXC_SHIFT);
      break;
    case CSR_FRM:
      dirty_fp_state;
      state.frm = val & (FSR_RD >> FSR_RD_SHIFT);
      break;
    case CSR_FCSR:
      dirty_fp_state;
      state.fflags = (val & FSR_AEXC) >> FSR_AEXC_SHIFT;
      state.frm = (val & FSR_RD) >> FSR_RD_SHIFT;
      break;
    case CSR_VCSR:
      dirty_vs_state;
      VU.vxsat = (val & VCSR_VXSAT) >> VCSR_VXSAT_SHIFT;
      VU.vxrm = (val & VCSR_VXRM) >> VCSR_VXRM_SHIFT;
      break;
    case CSR_MSTATUS: {
      bool has_page = supports_extension('S') && supports_impl(IMPL_MMU);
      if ((val ^ state.mstatus) &
          (MSTATUS_MPP | MSTATUS_MPRV
           | (has_page ? (MSTATUS_MXR | MSTATUS_SUM) : 0)
           | MSTATUS_MXR))
        mmu->flush_tlb();

      bool has_fs = supports_extension('S') || supports_extension('F')
                  || supports_extension('V');
      bool has_vs = supports_extension('V');
      bool has_mpv = supports_extension('S') && supports_extension('H');
      bool has_gva = has_mpv;

      reg_t mask = MSTATUS_MIE | MSTATUS_MPIE | MSTATUS_MPRV
                 | (supports_extension('S') ? (MSTATUS_SIE | MSTATUS_SPIE) : 0)
                 | MSTATUS_TW | MSTATUS_TSR
                 | (has_page ? (MSTATUS_MXR | MSTATUS_SUM | MSTATUS_TVM) : 0)
                 | (has_fs ? MSTATUS_FS : 0)
                 | (has_vs ? MSTATUS_VS : 0)
                 | (!custom_extensions.empty() ? MSTATUS_XS : 0)
                 | (has_gva ? MSTATUS_GVA : 0)
                 | (has_mpv ? MSTATUS_MPV : 0);

      reg_t requested_mpp = legalize_privilege(get_field(val, MSTATUS_MPP));
      state.mstatus = set_field(state.mstatus, MSTATUS_MPP, requested_mpp);
      if (supports_extension('S'))
        mask |= MSTATUS_SPP;

      state.mstatus = (state.mstatus & ~mask) | (val & mask);

      bool dirty = (state.mstatus & MSTATUS_FS) == MSTATUS_FS;
      dirty |= (state.mstatus & MSTATUS_XS) == MSTATUS_XS;
      dirty |= (state.mstatus & MSTATUS_VS) == MSTATUS_VS;
      if (max_xlen == 32)
        state.mstatus = set_field(state.mstatus, MSTATUS32_SD, dirty);
      else
        state.mstatus = set_field(state.mstatus, MSTATUS64_SD, dirty);

      if (supports_extension('U'))
        state.mstatus = set_field(state.mstatus, MSTATUS_UXL, xlen_to_uxl(max_xlen));
      if (supports_extension('S'))
        state.mstatus = set_field(state.mstatus, MSTATUS_SXL, xlen_to_uxl(max_xlen));
      // U-XLEN == S-XLEN == M-XLEN
      xlen = max_xlen;
      break;
    }
    case CSR_MIP: {
      reg_t mask = (supervisor_ints | hypervisor_ints) & (MIP_SSIP | MIP_STIP | vssip_int);
      state.mip = (state.mip & ~mask) | (val & mask);
      break;
    }
    case CSR_MIE:
      state.mie = (state.mie & ~all_ints) | (val & all_ints);
      break;
    case CSR_MIDELEG:
      state.mideleg = (state.mideleg & ~delegable_ints) | (val & delegable_ints);
      break;
    case CSR_MEDELEG: {
      reg_t mask =
        (1 << CAUSE_MISALIGNED_FETCH) |
        (1 << CAUSE_BREAKPOINT) |
        (1 << CAUSE_USER_ECALL) |
        (1 << CAUSE_SUPERVISOR_ECALL) |
        (1 << CAUSE_FETCH_PAGE_FAULT) |
        (1 << CAUSE_LOAD_PAGE_FAULT) |
        (1 << CAUSE_STORE_PAGE_FAULT);
      mask |= supports_extension('H') ? hypervisor_exceptions : 0;
      state.medeleg = (state.medeleg & ~mask) | (val & mask);
      break;
    }
    case CSR_MINSTRET:
    case CSR_MCYCLE:
      if (xlen == 32)
        state.minstret = (state.minstret >> 32 << 32) | (val & 0xffffffffU);
      else
        state.minstret = val;
      // The ISA mandates that if an instruction writes instret, the write
      // takes precedence over the increment to instret.  However, Spike
      // unconditionally increments instret after executing an instruction.
      // Correct for this artifact by decrementing instret here.
      state.minstret--;
      break;
    case CSR_MINSTRETH:
    case CSR_MCYCLEH:
      state.minstret = (val << 32) | (state.minstret << 32 >> 32);
      state.minstret--; // See comment above.
      break;
    case CSR_SCOUNTEREN:
      state.scounteren = val;
      break;
    case CSR_MCOUNTEREN:
      state.mcounteren = val;
      break;
    case CSR_SSTATUS: {
      reg_t mask = SSTATUS_SIE | SSTATUS_SPIE | SSTATUS_SPP | SSTATUS_FS
                 | SSTATUS_XS | SSTATUS_SUM | SSTATUS_MXR
                 | (supports_extension('V') ? SSTATUS_VS : 0);
      return set_csr(CSR_MSTATUS, (state.mstatus & ~mask) | (val & mask));
    }
    case CSR_SIP: {
      reg_t mask;
      if (state.v) {
        mask = state.hideleg & MIP_VSSIP;
        val = val << 1;
      } else {
        mask = state.mideleg & MIP_SSIP;
      }
      state.mip = (state.mip & ~mask) | (val & mask);
      break;
    }
    case CSR_SIE: {
      reg_t mask;
      if (state.v) {
        mask = state.hideleg & MIP_VS_MASK;
        val = val << 1;
      } else {
        mask = state.mideleg & ~MIP_HS_MASK;
      }
      state.mie = (state.mie & ~mask) | (val & mask);
      break;
    }
    case CSR_SATP:
      if (!supports_impl(IMPL_MMU))
        val = 0;

      if (satp_valid(val)) {
        mmu->flush_tlb();

        if (state.v)
          state.vsatp = compute_new_satp(val, state.vsatp);
        else
          state.satp = compute_new_satp(val, state.satp);
      }
      break;
    case CSR_SEPC:
      if (state.v)
        state.vsepc = val & ~(reg_t)1;
      else
        state.sepc = val & ~(reg_t)1;
      break;
    case CSR_STVEC:
      if (state.v)
        state.vstvec = val & ~(reg_t)2;
      else
        state.stvec = val & ~(reg_t)2;
      break;
    case CSR_SSCRATCH:
      if (state.v)
        state.vsscratch = val;
      else
        state.sscratch = val;
      break;
    case CSR_SCAUSE:
      if (state.v)
        state.vscause = val;
      else
        state.scause = val;
      break;
    case CSR_STVAL:
      if (state.v)
        state.vstval = val;
      else
        state.stval = val;
      break;
    case CSR_MEPC: state.mepc = val & ~(reg_t)1; break;
    case CSR_MTVEC: state.mtvec = val & ~(reg_t)2; break;
    case CSR_MSCRATCH: state.mscratch = val; break;
    case CSR_MCAUSE: state.mcause = val; break;
    case CSR_MTVAL: state.mtval = val; break;
    case CSR_MTVAL2: state.mtval2 = val; break;
    case CSR_MTINST: state.mtinst = val; break;
    case CSR_MISA: {
      // the write is ignored if increasing IALIGN would misalign the PC
      if (!(val & (1L << ('C' - 'A'))) && (state.pc & 2))
        break;

      if (!(val & (1L << ('F' - 'A'))))
        val &= ~(1L << ('D' - 'A'));

      // allow MAFDCHB bits in MISA to be modified
      reg_t mask = 0;
      mask |= 1L << ('M' - 'A');
      mask |= 1L << ('A' - 'A');
      mask |= 1L << ('F' - 'A');
      mask |= 1L << ('D' - 'A');
      mask |= 1L << ('C' - 'A');
      mask |= 1L << ('H' - 'A');
      mask |= 1L << ('B' - 'A');
      mask &= max_isa;

      state.misa = (val & mask) | (state.misa & ~mask);

      // update the forced bits in MIDELEG and other CSRs
      if (supports_extension('H'))
        state.mideleg |= MIDELEG_FORCED_MASK;
      else {
        state.mideleg &= ~MIDELEG_FORCED_MASK;
        state.medeleg &= ~hypervisor_exceptions;
        state.mstatus &= ~(MSTATUS_GVA | MSTATUS_MPV);
        state.mie &= ~MIP_HS_MASK;  // also takes care of hip, sip, hvip
        state.mip &= ~MIP_HS_MASK;  // also takes care of hie, sie
        set_csr(CSR_HSTATUS, 0);
      }
      break;
    }
    case CSR_HSTATUS: {
      reg_t mask = HSTATUS_VTSR | HSTATUS_VTW
                   | (supports_impl(IMPL_MMU) ? HSTATUS_VTVM : 0)
                   | HSTATUS_HU | HSTATUS_SPVP | HSTATUS_SPV | HSTATUS_GVA;
      state.hstatus = set_field(state.hstatus, HSTATUS_VSXL, xlen_to_uxl(max_xlen));
      state.hstatus = (state.hstatus & ~mask) | (val & mask);
      break;
    }
    case CSR_HEDELEG: {
      reg_t mask =
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
        (1 << CAUSE_STORE_PAGE_FAULT);
      state.hedeleg = (state.hedeleg & ~mask) | (val & mask);
      break;
    }
    case CSR_HIDELEG: {
      reg_t mask = MIP_VS_MASK;
      state.hideleg = (state.hideleg & ~mask) | (val & mask);
      break;
    }
    case CSR_HIE: {
      reg_t mask = MIP_HS_MASK;
      state.mie = (state.mie & ~mask) | (val & mask);
      break;
    }
    case CSR_HCOUNTEREN:
      state.hcounteren = val;
      break;
    case CSR_HGEIE:
      /* Ignore */
      break;
    case CSR_HTVAL:
      state.htval = val;
      break;
    case CSR_HIP: {
      reg_t mask = MIP_VSSIP;
      state.mip = (state.mip & ~mask) | (val & mask);
      break;
    }
    case CSR_HVIP: {
      reg_t mask = MIP_VS_MASK;
      state.mip = (state.mip & ~mask) | (val & mask);
      break;
    }
    case CSR_HTINST:
      state.htinst = val;
      break;
    case CSR_HGATP: {
      mmu->flush_tlb();

      reg_t mask;
      if (max_xlen == 32) {
        mask = HGATP32_PPN | HGATP32_MODE;
      } else {
        mask = HGATP64_PPN & ((reg_t(1) << (MAX_PADDR_BITS - PGSHIFT)) - 1);

        if (get_field(val, HGATP64_MODE) == HGATP_MODE_OFF ||
            get_field(val, HGATP64_MODE) == HGATP_MODE_SV39X4 ||
            get_field(val, HGATP64_MODE) == HGATP_MODE_SV48X4)
          mask |= HGATP64_MODE;
      }
      mask &= ~(reg_t)3;

      state.hgatp = val & mask;
      break;
    }
    case CSR_VSSTATUS: {
      reg_t mask = SSTATUS_VS_MASK;
      mask |= (supports_extension('V') ? SSTATUS_VS : 0);
      state.vsstatus = (state.vsstatus & ~mask) | (val & mask);
      state.vsstatus &= (xlen == 64 ? ~SSTATUS64_SD : ~SSTATUS32_SD);
      if (((state.vsstatus & SSTATUS_FS) == SSTATUS_FS) ||
          ((state.vsstatus & SSTATUS_VS) == SSTATUS_VS) ||
          ((state.vsstatus & SSTATUS_XS) == SSTATUS_XS)) {
         state.vsstatus |= (xlen == 64 ? SSTATUS64_SD : SSTATUS32_SD);
      }
      state.vsstatus = set_field(state.vsstatus, SSTATUS_UXL, xlen_to_uxl(max_xlen));
      break;
    }
    case CSR_VSIE: {
      reg_t mask = state.hideleg & MIP_VS_MASK;
      state.mie = (state.mie & ~mask) | ((val << 1) & mask);
      break;
    }
    case CSR_VSTVEC: state.vstvec = val & ~(reg_t)2; break;
    case CSR_VSSCRATCH: state.vsscratch = val; break;
    case CSR_VSEPC: state.vsepc = val & ~(reg_t)1; break;
    case CSR_VSCAUSE: state.vscause = val; break;
    case CSR_VSTVAL: state.vstval = val; break;
    case CSR_VSIP: {
      reg_t mask = state.hideleg & MIP_VSSIP;
      state.mip = (state.mip & ~mask) | ((val << 1) & mask);
      break;
    }
    case CSR_VSATP:
      if (!supports_impl(IMPL_MMU))
        val = 0;

      mmu->flush_tlb();
      state.vsatp = compute_new_satp(val, state.vsatp);
      break;
    case CSR_TSELECT:
      if (val < state.num_triggers) {
        state.tselect = val;
      }
      break;
    case CSR_TDATA1:
      {
        mcontrol_t *mc = &state.mcontrol[state.tselect];
        if (mc->dmode && !state.debug_mode) {
          break;
        }
        mc->dmode = get_field(val, MCONTROL_DMODE(xlen));
        mc->select = get_field(val, MCONTROL_SELECT);
        mc->timing = get_field(val, MCONTROL_TIMING);
        mc->action = (mcontrol_action_t) get_field(val, MCONTROL_ACTION);
        mc->chain = get_field(val, MCONTROL_CHAIN);
        mc->match = (mcontrol_match_t) get_field(val, MCONTROL_MATCH);
        mc->m = get_field(val, MCONTROL_M);
        mc->h = get_field(val, MCONTROL_H);
        mc->s = get_field(val, MCONTROL_S);
        mc->u = get_field(val, MCONTROL_U);
        mc->execute = get_field(val, MCONTROL_EXECUTE);
        mc->store = get_field(val, MCONTROL_STORE);
        mc->load = get_field(val, MCONTROL_LOAD);
        // Assume we're here because of csrw.
        if (mc->execute)
          mc->timing = 0;
        trigger_updated();
      }
      break;
    case CSR_TDATA2:
      if (state.mcontrol[state.tselect].dmode && !state.debug_mode) {
        break;
      }
      if (state.tselect < state.num_triggers) {
        state.tdata2[state.tselect] = val;
      }
      break;
    case CSR_DCSR:
      state.dcsr.prv = get_field(val, DCSR_PRV);
      state.dcsr.step = get_field(val, DCSR_STEP);
      // TODO: ndreset and fullreset
      state.dcsr.ebreakm = get_field(val, DCSR_EBREAKM);
      state.dcsr.ebreakh = get_field(val, DCSR_EBREAKH);
      state.dcsr.ebreaks = get_field(val, DCSR_EBREAKS);
      state.dcsr.ebreaku = get_field(val, DCSR_EBREAKU);
      state.dcsr.halt = get_field(val, DCSR_HALT);
      break;
    case CSR_DPC:
      state.dpc = val & ~(reg_t)1;
      break;
    case CSR_DSCRATCH0:
      state.dscratch0 = val;
      break;
    case CSR_DSCRATCH1:
      state.dscratch1 = val;
      break;
    case CSR_VSTART:
      dirty_vs_state;
      VU.vstart = val & (VU.get_vlen() - 1);
      break;
    case CSR_VXSAT:
      dirty_vs_state;
      VU.vxsat = val & 0x1ul;
      break;
    case CSR_VXRM:
      dirty_vs_state;
      VU.vxrm = val & 0x3ul;
      break;
  }

#if defined(RISCV_ENABLE_COMMITLOG)
  switch (which)
  {
    case CSR_FFLAGS:
      LOG_CSR(CSR_MSTATUS);
      LOG_CSR(CSR_FFLAGS);
      break;
    case CSR_FRM:
      LOG_CSR(CSR_MSTATUS);
      LOG_CSR(CSR_FRM);
      break;
    case CSR_FCSR:
      LOG_CSR(CSR_MSTATUS);
      LOG_CSR(CSR_FFLAGS);
      LOG_CSR(CSR_FRM);
      LOG_CSR(CSR_FCSR);
      break;
    case CSR_VCSR:
      LOG_CSR(CSR_MSTATUS);
      LOG_CSR(CSR_VXSAT);
      LOG_CSR(CSR_VXRM);
      break;

    case CSR_VSTART:
      LOG_CSR(CSR_MSTATUS);
      LOG_CSR(CSR_VSTART);
      break;
    case CSR_VXSAT:
      LOG_CSR(CSR_MSTATUS);
      LOG_CSR(CSR_VXSAT);
      break;
    case CSR_VXRM:
      LOG_CSR(CSR_MSTATUS);
      LOG_CSR(CSR_VXRM);
      break;

    case CSR_SSTATUS:
      LOG_CSR(CSR_MSTATUS);
      LOG_CSR(CSR_SSTATUS);
      break;
    case CSR_SIP:
      LOG_CSR(CSR_MIP);
      LOG_CSR(CSR_SIP);
      break;
    case CSR_SIE:
      LOG_CSR(CSR_MIE);
      LOG_CSR(CSR_SIE);
      break;

    case CSR_MSTATUS:
    case CSR_MIP:
    case CSR_MIE:
    case CSR_MIDELEG:
    case CSR_MEDELEG:
    case CSR_MINSTRET:
    case CSR_MCYCLE:
    case CSR_MINSTRETH:
    case CSR_MCYCLEH:
    case CSR_SCOUNTEREN:
    case CSR_MCOUNTEREN:
    case CSR_SATP:
    case CSR_SEPC:
    case CSR_STVEC:
    case CSR_SSCRATCH:
    case CSR_SCAUSE:
    case CSR_STVAL:
    case CSR_MEPC:
    case CSR_MTVEC:
    case CSR_MSCRATCH:
    case CSR_MCAUSE:
    case CSR_MTVAL:
    case CSR_MISA:
    case CSR_TSELECT:
    case CSR_TDATA1:
    case CSR_TDATA2:
    case CSR_DCSR:
    case CSR_DPC:
    case CSR_DSCRATCH0:
    case CSR_DSCRATCH1:
    case CSR_SENTROPY:
      LOG_CSR(which);
      break;
  }
#endif
}

// Note that get_csr is sometimes called when read side-effects should not
// be actioned.  In other words, Spike cannot currently support CSRs with
// side effects on reads.
reg_t processor_t::get_csr(int which, insn_t insn, bool write, bool peek)
{
  uint32_t ctr_en = -1;
  if (state.prv < PRV_M)
    ctr_en &= state.mcounteren;
  if (supports_extension('S') && state.prv < PRV_S)
    ctr_en &= state.scounteren;
  bool ctr_ok = (ctr_en >> (which & 31)) & 1;
  if (state.v)
    ctr_en &= state.hcounteren;
  bool ctr_v_ok = (ctr_en >> (which & 31)) & 1;

  reg_t res = 0;
#define ret(n) do { \
    res = (n); \
    goto out; \
  } while (false)

  if ((which >= CSR_HPMCOUNTER3 && which <= CSR_HPMCOUNTER31) ||
      (xlen == 32 && which >= CSR_HPMCOUNTER3H && which <= CSR_HPMCOUNTER31H)) {
    if (!ctr_ok)
      goto throw_illegal;
    if (!ctr_v_ok)
      goto throw_virtual;
    ret(0);
  }
  if (which >= CSR_MHPMCOUNTER3 && which <= CSR_MHPMCOUNTER31)
    ret(0);
  if (xlen == 32 && which >= CSR_MHPMCOUNTER3H && which <= CSR_MHPMCOUNTER31H)
    ret(0);
  if (which >= CSR_MHPMEVENT3 && which <= CSR_MHPMEVENT31)
    ret(0);

  if (which >= CSR_PMPADDR0 && which < CSR_PMPADDR0 + state.max_pmp) {
    // If n_pmp is zero, that means pmp is not implemented hence raise trap if it tries to access the csr
    if (n_pmp == 0)
      goto throw_illegal;
    reg_t i = which - CSR_PMPADDR0;
    if ((state.pmpcfg[i] & PMP_A) >= PMP_NAPOT)
      ret(state.pmpaddr[i] | (~pmp_tor_mask() >> 1));
    else
      ret(state.pmpaddr[i] & pmp_tor_mask());
  }

  if (which >= CSR_PMPCFG0 && which < CSR_PMPCFG0 + state.max_pmp / 4) {
    require((which & ((xlen / 32) - 1)) == 0);

    reg_t cfg_res = 0;
    for (size_t i0 = (which - CSR_PMPCFG0) * 4, i = i0; i < i0 + xlen / 8 && i < state.max_pmp; i++)
      cfg_res |= reg_t(state.pmpcfg[i]) << (8 * (i - i0));
    ret(cfg_res);
  }

  switch (which)
  {
    case CSR_SENTROPY:
      if (!supports_extension('K'))
        break;
      /* Read-only access disallowed due to wipe-on-read side effect */
      if (!write)
        break;
      ret(es.get_sentropy());
    case CSR_FFLAGS:
      require_fp;
      if (!supports_extension('F'))
        break;
      ret(state.fflags);
    case CSR_FRM:
      require_fp;
      if (!supports_extension('F'))
        break;
      ret(state.frm);
    case CSR_FCSR:
      require_fp;
      if (!supports_extension('F'))
        break;
      ret((state.fflags << FSR_AEXC_SHIFT) | (state.frm << FSR_RD_SHIFT));
    case CSR_VCSR:
      require_vector_vs;
      if (!supports_extension('V'))
        break;
      ret((VU.vxsat << VCSR_VXSAT_SHIFT) | (VU.vxrm << VCSR_VXRM_SHIFT));
    case CSR_INSTRET:
    case CSR_CYCLE:
      if (!ctr_ok)
        goto throw_illegal;
      if (!ctr_v_ok)
        goto throw_virtual;
      ret(state.minstret);
    case CSR_MINSTRET:
    case CSR_MCYCLE:
      ret(state.minstret);
    case CSR_INSTRETH:
    case CSR_CYCLEH:
      if (!ctr_ok || xlen != 32)
        goto throw_illegal;
      if (!ctr_v_ok)
        goto throw_virtual;
      ret(state.minstret >> 32);
    case CSR_MINSTRETH:
    case CSR_MCYCLEH:
      if (xlen == 32)
        ret(state.minstret >> 32);
      break;
    case CSR_SCOUNTEREN: ret(state.scounteren);
    case CSR_MCOUNTEREN:
      if (!supports_extension('U'))
        break;
      ret(state.mcounteren);
    case CSR_MCOUNTINHIBIT: ret(0);
    case CSR_SSTATUS: {
      reg_t mask = SSTATUS_SIE | SSTATUS_SPIE | SSTATUS_UBE | SSTATUS_SPP
                 | SSTATUS_FS | (supports_extension('V') ? SSTATUS_VS : 0)
                 | SSTATUS_XS | SSTATUS_SUM | SSTATUS_MXR | SSTATUS_UXL;
      reg_t sstatus = state.mstatus & mask;
      if ((sstatus & SSTATUS_FS) == SSTATUS_FS ||
          (sstatus & SSTATUS_XS) == SSTATUS_XS ||
          (sstatus & SSTATUS_VS) == SSTATUS_VS)
        sstatus |= (xlen == 32 ? SSTATUS32_SD : SSTATUS64_SD);
      ret(sstatus);
    }
    case CSR_SIP: {
      if (state.v) {
        ret((state.mip & state.hideleg & MIP_VS_MASK) >> 1);
      } else {
        ret(state.mip & state.mideleg & ~MIP_HS_MASK);
      }
    }
    case CSR_SIE: {
      if (state.v) {
        ret((state.mie & state.hideleg & MIP_VS_MASK) >> 1);
      } else {
        ret(state.mie & state.mideleg & ~MIP_HS_MASK);
      }
    }
    case CSR_SEPC: {
      if (state.v) {
        ret(state.vsepc & pc_alignment_mask());
      } else {
        ret(state.sepc & pc_alignment_mask());
      }
    }
    case CSR_STVAL: {
      if (state.v) {
        ret(state.vstval);
      } else {
        ret(state.stval);
      }
    }
    case CSR_STVEC: {
      if (state.v) {
        ret(state.vstvec);
      } else {
        ret(state.stvec);
      }
    }
    case CSR_SCAUSE: {
      if (state.v) {
        if (max_xlen > xlen)
          ret(state.vscause | ((state.vscause >> (max_xlen-1)) << (xlen-1)));
        ret(state.vscause);
      } else {
        if (max_xlen > xlen)
          ret(state.scause | ((state.scause >> (max_xlen-1)) << (xlen-1)));
        ret(state.scause);
      }
    }
    case CSR_SATP: {
      if (state.v) {
        if (get_field(state.hstatus, HSTATUS_VTVM))
          goto throw_virtual;
        ret(state.vsatp);
      } else {
        if (get_field(state.mstatus, MSTATUS_TVM))
          require_privilege(PRV_M);
        ret(state.satp);
      }
    }
    case CSR_SSCRATCH: {
      if (state.v) {
        ret(state.vsscratch);
      } else {
        ret(state.sscratch);
      }
    }
    case CSR_MSTATUS: ret(state.mstatus);
    case CSR_MSTATUSH:
      if (xlen == 32)
        ret((state.mstatus >> 32) & (MSTATUSH_SBE | MSTATUSH_MBE));
      break;
    case CSR_MIP: ret(state.mip);
    case CSR_MIE: ret(state.mie);
    case CSR_MEPC: ret(state.mepc & pc_alignment_mask());
    case CSR_MSCRATCH: ret(state.mscratch);
    case CSR_MCAUSE: ret(state.mcause);
    case CSR_MTVAL: ret(state.mtval);
    case CSR_MTVAL2:
      if (supports_extension('H'))
        ret(state.mtval2);
      break;
    case CSR_MTINST:
      if (supports_extension('H'))
        ret(state.mtinst);
      break;
    case CSR_MISA: ret(state.misa);
    case CSR_MARCHID: ret(5);
    case CSR_MIMPID: ret(0);
    case CSR_MVENDORID: ret(0);
    case CSR_MHARTID: ret(id);
    case CSR_MTVEC: ret(state.mtvec);
    case CSR_MEDELEG:
      if (!supports_extension('S'))
        break;
      ret(state.medeleg);
    case CSR_MIDELEG:
      if (!supports_extension('S'))
        break;
      ret(state.mideleg);
    case CSR_HSTATUS: ret(state.hstatus);
    case CSR_HEDELEG: ret(state.hedeleg);
    case CSR_HIDELEG: ret(state.hideleg);
    case CSR_HIE: ret(state.mie & MIP_HS_MASK);
    case CSR_HCOUNTEREN: ret(state.hcounteren);
    case CSR_HGEIE: ret(0);
    case CSR_HTVAL: ret(state.htval);
    case CSR_HIP: ret(state.mip & MIP_HS_MASK);
    case CSR_HVIP: ret(state.mip & MIP_VS_MASK);
    case CSR_HTINST: ret(state.htinst);
    case CSR_HGATP: {
      if (!state.v && get_field(state.mstatus, MSTATUS_TVM))
        require_privilege(PRV_M);
      ret(state.hgatp);
    }
    case CSR_HGEIP: ret(0);
    case CSR_VSSTATUS: {
      reg_t mask = SSTATUS_VS_MASK;
      mask |= (supports_extension('V') ? SSTATUS_VS : 0);
      mask |= (xlen == 64 ? SSTATUS64_SD : SSTATUS32_SD);
      ret(state.vsstatus & mask);
    }
    case CSR_VSIE: ret((state.mie & state.hideleg & MIP_VS_MASK) >> 1);
    case CSR_VSTVEC: ret(state.vstvec);
    case CSR_VSSCRATCH: ret(state.vsscratch);
    case CSR_VSEPC: ret(state.vsepc & pc_alignment_mask());
    case CSR_VSCAUSE: ret(state.vscause);
    case CSR_VSTVAL: ret(state.vstval);
    case CSR_VSIP: ret((state.mip & state.hideleg & MIP_VS_MASK) >> 1);
    case CSR_VSATP: ret(state.vsatp);
    case CSR_TSELECT: ret(state.tselect);
    case CSR_TDATA1:
      if (state.tselect < state.num_triggers) {
        reg_t v = 0;
        mcontrol_t *mc = &state.mcontrol[state.tselect];
        v = set_field(v, MCONTROL_TYPE(xlen), mc->type);
        v = set_field(v, MCONTROL_DMODE(xlen), mc->dmode);
        v = set_field(v, MCONTROL_MASKMAX(xlen), mc->maskmax);
        v = set_field(v, MCONTROL_SELECT, mc->select);
        v = set_field(v, MCONTROL_TIMING, mc->timing);
        v = set_field(v, MCONTROL_ACTION, mc->action);
        v = set_field(v, MCONTROL_CHAIN, mc->chain);
        v = set_field(v, MCONTROL_MATCH, mc->match);
        v = set_field(v, MCONTROL_M, mc->m);
        v = set_field(v, MCONTROL_H, mc->h);
        v = set_field(v, MCONTROL_S, mc->s);
        v = set_field(v, MCONTROL_U, mc->u);
        v = set_field(v, MCONTROL_EXECUTE, mc->execute);
        v = set_field(v, MCONTROL_STORE, mc->store);
        v = set_field(v, MCONTROL_LOAD, mc->load);
        ret(v);
      } else {
        ret(0);
      }
      break;
    case CSR_TDATA2:
      if (state.tselect < state.num_triggers) {
        ret(state.tdata2[state.tselect]);
      } else {
        ret(0);
      }
      break;
    case CSR_TDATA3: ret(0);
    case CSR_DCSR:
      {
        if (!state.debug_mode)
          break;
        uint32_t v = 0;
        v = set_field(v, DCSR_XDEBUGVER, 1);
        v = set_field(v, DCSR_EBREAKM, state.dcsr.ebreakm);
        v = set_field(v, DCSR_EBREAKH, state.dcsr.ebreakh);
        v = set_field(v, DCSR_EBREAKS, state.dcsr.ebreaks);
        v = set_field(v, DCSR_EBREAKU, state.dcsr.ebreaku);
        v = set_field(v, DCSR_STOPCYCLE, 0);
        v = set_field(v, DCSR_STOPTIME, 0);
        v = set_field(v, DCSR_CAUSE, state.dcsr.cause);
        v = set_field(v, DCSR_STEP, state.dcsr.step);
        v = set_field(v, DCSR_PRV, state.dcsr.prv);
        ret(v);
      }
    case CSR_DPC:
      if (!state.debug_mode)
        break;
      ret(state.dpc & pc_alignment_mask());
    case CSR_DSCRATCH0:
      if (!state.debug_mode)
        break;
      ret(state.dscratch0);
    case CSR_DSCRATCH1:
      if (!state.debug_mode)
        break;
      ret(state.dscratch1);
    case CSR_VSTART:
      require_vector_vs;
      if (!supports_extension('V'))
        break;
      ret(VU.vstart);
    case CSR_VXSAT:
      require_vector_vs;
      if (!supports_extension('V'))
        break;
      ret(VU.vxsat);
    case CSR_VXRM:
      require_vector_vs;
      if (!supports_extension('V'))
        break;
      ret(VU.vxrm);
    case CSR_VL:
      require_vector_vs;
      if (!supports_extension('V'))
        break;
      ret(VU.vl);
    case CSR_VTYPE:
      require_vector_vs;
      if (!supports_extension('V'))
        break;
      ret(VU.vtype);
    case CSR_VLENB:
      require_vector_vs;
      if (!supports_extension('V'))
        break;
      ret(VU.vlenb);
  }

#undef ret

  // If we get here, the CSR doesn't exist.  Unimplemented CSRs always throw
  // illegal-instruction exceptions, not virtual-instruction exceptions.
throw_illegal:
  throw trap_illegal_instruction(insn.bits());

throw_virtual:
  throw trap_virtual_instruction(insn.bits());

out:
  // Check permissions.  Raise virtual-instruction exception if V=1,
  // privileges are insufficient, and the CSR belongs to supervisor or
  // hypervisor.  Raise illegal-instruction exception otherwise.

  if (peek)
    return res;

  unsigned csr_priv = get_field(which, 0x300);
  unsigned priv = state.prv == PRV_S && !state.v ? PRV_HS : state.prv;

  if ((csr_priv == PRV_S && !supports_extension('S')) ||
      (csr_priv == PRV_HS && !supports_extension('H')))
    goto throw_illegal;

  if (priv < csr_priv) {
    if (state.v && csr_priv <= PRV_HS)
      goto throw_virtual;
    goto throw_illegal;
  }

  return res;
}

reg_t illegal_instruction(processor_t* p, insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction(insn.bits());
}

insn_func_t processor_t::decode_insn(insn_t insn)
{
  // look up opcode in hash table
  size_t idx = insn.bits() % OPCODE_CACHE_SIZE;
  insn_desc_t desc = opcode_cache[idx];

  if (unlikely(insn.bits() != desc.match || !(xlen == 64 ? desc.rv64 : desc.rv32))) {
    // fall back to linear search
    int cnt = 0;
    insn_desc_t* p = &instructions[0];
    while ((insn.bits() & p->mask) != p->match || !(xlen == 64 ? p->rv64 : p->rv32))
      p++, cnt++;
    desc = *p;

    if (p->mask != 0 && p > &instructions[0]) {
      if (p->match != (p-1)->match && p->match != (p+1)->match) {
        // move to front of opcode list to reduce miss penalty
        while (--p >= &instructions[0])
          *(p+1) = *p;
        instructions[0] = desc;
      }
    }

    opcode_cache[idx] = desc;
    opcode_cache[idx].match = insn.bits();
  }

  return xlen == 64 ? desc.rv64 : desc.rv32;
}

void processor_t::register_insn(insn_desc_t desc)
{
  instructions.push_back(desc);
}

void processor_t::build_opcode_map()
{
  struct cmp {
    bool operator()(const insn_desc_t& lhs, const insn_desc_t& rhs) {
      if (lhs.match == rhs.match)
        return lhs.mask > rhs.mask;
      return lhs.match > rhs.match;
    }
  };
  std::sort(instructions.begin(), instructions.end(), cmp());

  for (size_t i = 0; i < OPCODE_CACHE_SIZE; i++)
    opcode_cache[i] = {0, 0, &illegal_instruction, &illegal_instruction};
}

void processor_t::register_extension(extension_t* x)
{
  for (auto insn : x->get_instructions())
    register_insn(insn);
  build_opcode_map();

  if (disassembler)
    for (auto disasm_insn : x->get_disasms())
      disassembler->add_insn(disasm_insn);

  if (!custom_extensions.insert(std::make_pair(x->name(), x)).second) {
    fprintf(stderr, "extensions must have unique names (got two named \"%s\"!)\n", x->name());
    abort();
  }

  x->set_processor(this);
}

void processor_t::register_base_instructions()
{
  #define DECLARE_INSN(name, match, mask) \
    insn_bits_t name##_match = (match), name##_mask = (mask);
  #include "encoding.h"
  #undef DECLARE_INSN

  #define DEFINE_INSN(name) \
    extern reg_t rv32_##name(processor_t*, insn_t, reg_t); \
    extern reg_t rv64_##name(processor_t*, insn_t, reg_t); \
    register_insn((insn_desc_t){ \
      name##_match, \
      name##_mask, \
      rv32_##name, \
      rv64_##name});
  #include "insn_list.h"
  #undef DEFINE_INSN

  register_insn({0, 0, &illegal_instruction, &illegal_instruction});
  build_opcode_map();
}

bool processor_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  switch (addr)
  {
    case 0:
      if (len <= 4) {
        memset(bytes, 0, len);
        bytes[0] = get_field(state.mip, MIP_MSIP);
        return true;
      }
      break;
  }

  return false;
}

bool processor_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  switch (addr)
  {
    case 0:
      if (len <= 4) {
        state.mip = set_field(state.mip, MIP_MSIP, bytes[0]);
        return true;
      }
      break;
  }

  return false;
}

void processor_t::trigger_updated()
{
  mmu->flush_tlb();
  mmu->check_triggers_fetch = false;
  mmu->check_triggers_load = false;
  mmu->check_triggers_store = false;

  for (unsigned i = 0; i < state.num_triggers; i++) {
    if (state.mcontrol[i].execute) {
      mmu->check_triggers_fetch = true;
    }
    if (state.mcontrol[i].load) {
      mmu->check_triggers_load = true;
    }
    if (state.mcontrol[i].store) {
      mmu->check_triggers_store = true;
    }
  }
}
