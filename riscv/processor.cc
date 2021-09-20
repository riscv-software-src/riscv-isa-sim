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
#include <iomanip>
#include <assert.h>
#include <limits.h>
#include <stdexcept>
#include <string>
#include <algorithm>

#undef STATE
#define STATE state

processor_t::processor_t(const char* isa, const char* priv, const char* varch,
                         simif_t* sim, uint32_t id, bool halt_on_reset,
                         FILE* log_file, std::ostream& sout_)
  : debug(false), halt_request(HR_NONE), sim(sim), id(id), xlen(0),
  histogram_enabled(false), log_commits_enabled(false),
  log_file(log_file), sout_(sout_.rdbuf()), halt_on_reset(halt_on_reset),
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
  isa_string = strtolower(str);
  const char* all_subsets = "imafdqchp"
#ifdef __SIZEOF_INT128__
    "v"
#endif
    "";

  max_isa = reg_t(2) << 62;
  if (isa_string.compare(0, 4, "rv32") == 0)
    max_xlen = 32, max_isa = reg_t(1) << 30;
  else if (isa_string.compare(0, 4, "rv64") == 0)
    max_xlen = 64;
  else
    bad_isa_string(str, "Spike supports either RV32I or RV64I");
  if (isa_string[4] == 'g')
    isa_string = isa_string.substr(0, 4) + "imafd" + isa_string.substr(5);
  if (isa_string[4] != 'i')
    bad_isa_string(str, "'I' extension is required");

  auto p = isa_string.begin();
  for (p += 4; islower(*p) && !strchr("zsx", *p); ++p) {
    while (*all_subsets && (*p != *all_subsets))
      ++all_subsets;
    if (!*all_subsets)
      bad_isa_string(str, "Wrong order");
    switch (*p) {
      case 'q': max_isa |= 1L << ('d' - 'a');
      case 'd': max_isa |= 1L << ('f' - 'a');
    }
    max_isa |= 1L << (*p - 'a');
    extension_table[toupper(*p)] = true;
    while (isdigit(*(p + 1))) {
      ++p; // skip major version, point, and minor version if presented
      if (*(p + 1) == 'p') ++p;
    }
    p += *(p + 1) == '_'; // underscores may be used to improve readability
  }

  while (islower(*p) || (*p == '_')) {
    p += *p == '_'; // first underscore is optional
    auto end = p;
    do ++end; while (*end && *end != '_');
    auto ext_str = std::string(p, end);
    if (ext_str == "zfh") {
      if (!((max_isa >> ('f' - 'a')) & 1))
        bad_isa_string(str, "'Zfh' extension requires 'F'");
      extension_table[EXT_ZFH] = true;
    } else if (ext_str == "zicsr") {
      // Spike necessarily has Zicsr, because
      // Zicsr is implied by the privileged architecture
    } else if (ext_str == "zba") {
      extension_table[EXT_ZBA] = true;
    } else if (ext_str == "zbb") {
      extension_table[EXT_ZBB] = true;
    } else if (ext_str == "zbc") {
      extension_table[EXT_ZBC] = true;
    } else if (ext_str == "zbs") {
      extension_table[EXT_ZBS] = true;
    } else if (ext_str == "zbkb") {
      extension_table[EXT_ZBKB] = true;
    } else if (ext_str == "zbkc") {
      extension_table[EXT_ZBKC] = true;
    } else if (ext_str == "zbkx") {
      extension_table[EXT_ZBKX] = true;
    } else if (ext_str == "zk") {
      extension_table[EXT_ZBKB] = true;
      extension_table[EXT_ZBKC] = true;
      extension_table[EXT_ZBKX] = true;
      extension_table[EXT_ZKND] = true;
      extension_table[EXT_ZKNE] = true;
      extension_table[EXT_ZKNH] = true;
      extension_table[EXT_ZKR] = true;
    } else if (ext_str == "zkn") {
      extension_table[EXT_ZBKB] = true;
      extension_table[EXT_ZBKC] = true;
      extension_table[EXT_ZBKX] = true;
      extension_table[EXT_ZKND] = true;
      extension_table[EXT_ZKNE] = true;
      extension_table[EXT_ZKNH] = true;
    } else if (ext_str == "zknd") {
      extension_table[EXT_ZKND] = true;
    } else if (ext_str == "zkne") {
      extension_table[EXT_ZKNE] = true;
    } else if (ext_str == "zknh") {
      extension_table[EXT_ZKNH] = true;
    } else if (ext_str == "zks") {
      extension_table[EXT_ZBKB] = true;
      extension_table[EXT_ZBKC] = true;
      extension_table[EXT_ZBKX] = true;
      extension_table[EXT_ZKSED] = true;
      extension_table[EXT_ZKSH] = true;
    } else if (ext_str == "zksed") {
      extension_table[EXT_ZKSED] = true;
    } else if (ext_str == "zksh") {
      extension_table[EXT_ZKSH] = true;
    } else if (ext_str == "zkr") {
      extension_table[EXT_ZKR] = true;
    } else if (ext_str == "zkt") {
    } else if (ext_str == "svnapot") {
      extension_table[EXT_SVNAPOT] = true;
    } else if (ext_str == "svpbmt") {
      extension_table[EXT_SVPBMT] = true;
    } else if (ext_str == "svinval") {
      extension_table[EXT_SVINVAL] = true;
    } else if (ext_str[0] == 'x') {
      max_isa |= 1L << ('x' - 'a');
      extension_table[toupper('x')] = true;
      if (ext_str == "xbitmanip") {
        extension_table[EXT_XBITMANIP] = true;
      } else if (ext_str.size() == 1) {
        bad_isa_string(str, "single 'X' is not a proper name");
      } else if (ext_str != "xdummy") {
        register_extension(find_extension(ext_str.substr(1).c_str())());
      }
    } else {
      bad_isa_string(str, ("unsupported extension: " + ext_str).c_str());
    }
    p = end;
  }
  if (*p) {
    bad_isa_string(str, ("can't parse: " + std::string(p, isa_string.end())).c_str());
  }
}

void state_t::reset(processor_t* const proc, reg_t max_isa)
{
  pc = DEFAULT_RSTVEC;
  XPR.reset();
  FPR.reset();

  // This assumes xlen is always max_xlen, which is true today (see
  // mstatus_csr_t::unlogged_write()):
  auto xlen = proc->get_max_xlen();

  prv = PRV_M;
  v = false;
  csrmap[CSR_MISA] = misa = std::make_shared<misa_csr_t>(proc, CSR_MISA, max_isa);
  csrmap[CSR_MSTATUS] = mstatus = std::make_shared<mstatus_csr_t>(proc, CSR_MSTATUS);
  csrmap[CSR_MEPC] = mepc = std::make_shared<epc_csr_t>(proc, CSR_MEPC);
  csrmap[CSR_MTVAL] = mtval = std::make_shared<basic_csr_t>(proc, CSR_MTVAL, 0);
  csrmap[CSR_MSCRATCH] = std::make_shared<basic_csr_t>(proc, CSR_MSCRATCH, 0);
  csrmap[CSR_MTVEC] = mtvec = std::make_shared<tvec_csr_t>(proc, CSR_MTVEC);
  csrmap[CSR_MCAUSE] = mcause = std::make_shared<cause_csr_t>(proc, CSR_MCAUSE);
  minstret = 0;
  csrmap[CSR_MIE] = mie = std::make_shared<mie_csr_t>(proc, CSR_MIE);
  csrmap[CSR_MIP] = mip = std::make_shared<mip_csr_t>(proc, CSR_MIP);
  auto sip_sie_accr = std::make_shared<generic_int_accessor_t>(this,
                                                               ~MIP_HS_MASK,  // read_mask
                                                               MIP_SSIP,      // ip_write_mask
                                                               ~MIP_HS_MASK,  // ie_write_mask
                                                               true,          // mask_mideleg
                                                               false,         // mask_hideleg
                                                               0);            // shiftamt

  auto hip_hie_accr = std::make_shared<generic_int_accessor_t>(this,
                                                               MIP_HS_MASK,   // read_mask
                                                               MIP_VSSIP,     // ip_write_mask
                                                               MIP_HS_MASK,   // ie_write_mask
                                                               false,         // mask_mideleg
                                                               false,         // mask_hideleg
                                                               0);

  auto hvip_accr = std::make_shared<generic_int_accessor_t>(this,
                                                            MIP_VS_MASK,   // read_mask
                                                            MIP_VS_MASK,   // ip_write_mask
                                                            MIP_VS_MASK,   // ie_write_mask
                                                            false,         // mask_mideleg
                                                            false,         // mask_hideleg
                                                            0);            // shiftamt

  auto vsip_vsie_accr = std::make_shared<generic_int_accessor_t>(this,
                                                                 MIP_VS_MASK,   // read_mask
                                                                 MIP_VSSIP,     // ip_write_mask
                                                                 MIP_VSSIP,     // ie_write_mask
                                                                 false,         // mask_mideleg
                                                                 true,          // mask_hideleg
                                                                 1);            // shiftamt

  auto nonvirtual_sip = std::make_shared<mip_proxy_csr_t>(proc, CSR_SIP, sip_sie_accr);
  auto vsip = std::make_shared<mip_proxy_csr_t>(proc, CSR_VSIP, vsip_vsie_accr);
  csrmap[CSR_VSIP] = vsip;
  csrmap[CSR_SIP] = std::make_shared<virtualized_csr_t>(proc, nonvirtual_sip, vsip);
  csrmap[CSR_HIP] = std::make_shared<mip_proxy_csr_t>(proc, CSR_HIP, hip_hie_accr);
  csrmap[CSR_HVIP] = std::make_shared<mip_proxy_csr_t>(proc, CSR_HVIP, hvip_accr);

  auto nonvirtual_sie = std::make_shared<mie_proxy_csr_t>(proc, CSR_SIE, sip_sie_accr);
  auto vsie = std::make_shared<mie_proxy_csr_t>(proc, CSR_VSIE, vsip_vsie_accr);
  csrmap[CSR_VSIE] = vsie;
  csrmap[CSR_SIE] = std::make_shared<virtualized_csr_t>(proc, nonvirtual_sie, vsie);
  csrmap[CSR_HIE] = std::make_shared<mie_proxy_csr_t>(proc, CSR_HIE, hip_hie_accr);

  csrmap[CSR_MEDELEG] = medeleg = std::make_shared<medeleg_csr_t>(proc, CSR_MEDELEG);
  csrmap[CSR_MIDELEG] = mideleg = std::make_shared<mideleg_csr_t>(proc, CSR_MIDELEG);
  mcounteren = std::make_shared<counteren_csr_t>(proc, CSR_MCOUNTEREN);
  if (proc->extension_enabled_const('U')) csrmap[CSR_MCOUNTEREN] = mcounteren;
  csrmap[CSR_SCOUNTEREN] = scounteren = std::make_shared<counteren_csr_t>(proc, CSR_SCOUNTEREN);
  auto nonvirtual_sepc = std::make_shared<epc_csr_t>(proc, CSR_SEPC);
  csrmap[CSR_VSEPC] = vsepc = std::make_shared<epc_csr_t>(proc, CSR_VSEPC);
  csrmap[CSR_SEPC] = sepc = std::make_shared<virtualized_csr_t>(proc, nonvirtual_sepc, vsepc);
  auto nonvirtual_stval = std::make_shared<basic_csr_t>(proc, CSR_STVAL, 0);
  csrmap[CSR_VSTVAL] = vstval = std::make_shared<basic_csr_t>(proc, CSR_VSTVAL, 0);
  csrmap[CSR_STVAL] = stval = std::make_shared<virtualized_csr_t>(proc, nonvirtual_stval, vstval);
  auto sscratch = std::make_shared<basic_csr_t>(proc, CSR_SSCRATCH, 0);
  auto vsscratch = std::make_shared<basic_csr_t>(proc, CSR_VSSCRATCH, 0);
  // Note: if max_isa does not include H, we don't really need this virtualized_csr_t at all (though it doesn't hurt):
  csrmap[CSR_SSCRATCH] = std::make_shared<virtualized_csr_t>(proc, sscratch, vsscratch);
  csrmap[CSR_VSSCRATCH] = vsscratch;
  auto nonvirtual_stvec = std::make_shared<tvec_csr_t>(proc, CSR_STVEC);
  csrmap[CSR_VSTVEC] = vstvec = std::make_shared<tvec_csr_t>(proc, CSR_VSTVEC);
  csrmap[CSR_STVEC] = stvec = std::make_shared<virtualized_csr_t>(proc, nonvirtual_stvec, vstvec);
  auto nonvirtual_satp = std::make_shared<satp_csr_t>(proc, CSR_SATP);
  csrmap[CSR_VSATP] = vsatp = std::make_shared<base_atp_csr_t>(proc, CSR_VSATP);
  csrmap[CSR_SATP] = satp = std::make_shared<virtualized_satp_csr_t>(proc, nonvirtual_satp, vsatp);
  auto nonvirtual_scause = std::make_shared<cause_csr_t>(proc, CSR_SCAUSE);
  csrmap[CSR_VSCAUSE] = vscause = std::make_shared<cause_csr_t>(proc, CSR_VSCAUSE);
  csrmap[CSR_SCAUSE] = scause = std::make_shared<virtualized_csr_t>(proc, nonvirtual_scause, vscause);
  mtval2 = 0;
  mtinst = 0;
  csrmap[CSR_HSTATUS] = hstatus = std::make_shared<hstatus_csr_t>(proc, CSR_HSTATUS);
  hideleg = 0;
  hedeleg = 0;
  csrmap[CSR_HCOUNTEREN] = hcounteren = std::make_shared<counteren_csr_t>(proc, CSR_HCOUNTEREN);
  htval = 0;
  htinst = 0;
  hgatp = 0;
  auto nonvirtual_sstatus = std::make_shared<sstatus_proxy_csr_t>(proc, CSR_SSTATUS, mstatus);
  csrmap[CSR_VSSTATUS] = vsstatus = std::make_shared<vsstatus_csr_t>(proc, CSR_VSSTATUS);
  csrmap[CSR_SSTATUS] = sstatus = std::make_shared<sstatus_csr_t>(proc, nonvirtual_sstatus, vsstatus);

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

  for (int i=0; i < max_pmp; ++i) {
    csrmap[CSR_PMPADDR0 + i] = pmpaddr[i] = std::make_shared<pmpaddr_csr_t>(proc, CSR_PMPADDR0 + i);
  }
  for (int i=0; i < max_pmp; i += xlen/8) {
    reg_t addr = CSR_PMPCFG0 + i/4;
    csrmap[addr] = std::make_shared<pmpcfg_csr_t>(proc, addr);
  }

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
  xlen = max_xlen;
  state.reset(this, max_isa);
  state.dcsr.halt = halt_on_reset;
  halt_on_reset = false;
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
  // Do nothing if no pending interrupts
  if (!pending_interrupts) {
    return;
  }

  // M-ints have higher priority over HS-ints and VS-ints
  const reg_t mie = get_field(state.mstatus->read(), MSTATUS_MIE);
  const reg_t m_enabled = state.prv < PRV_M || (state.prv == PRV_M && mie);
  reg_t enabled_interrupts = pending_interrupts & ~state.mideleg->read() & -m_enabled;
  if (enabled_interrupts == 0) {
    // HS-ints have higher priority over VS-ints
    const reg_t deleg_to_hs = state.mideleg->read() & ~state.hideleg;
    const reg_t sie = get_field(state.sstatus->read(), MSTATUS_SIE);
    const reg_t hs_enabled = state.v || state.prv < PRV_S || (state.prv == PRV_S && sie);
    enabled_interrupts = pending_interrupts & deleg_to_hs & -hs_enabled;
    if (state.v && enabled_interrupts == 0) {
      // VS-ints have least priority and can only be taken with virt enabled
      const reg_t deleg_to_vs = state.mideleg->read() & state.hideleg;
      const reg_t vs_enabled = state.prv < PRV_S || (state.prv == PRV_S && sie);
      enabled_interrupts = pending_interrupts & deleg_to_vs & -vs_enabled;
    }
  }

  if (!state.debug_mode && enabled_interrupts) {
    // nonstandard interrupts have highest priority
    if (enabled_interrupts >> (IRQ_M_EXT + 1))
      enabled_interrupts = enabled_interrupts >> (IRQ_M_EXT + 1) << (IRQ_M_EXT + 1);
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

  if (!extension_enabled('U'))
    return PRV_M;

  if (prv == PRV_HS || (prv == PRV_S && !extension_enabled('S')))
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
     *
     * The virtualized sstatus register also relies on this TLB flush,
     * since changing V might change sstatus.MXR and sstatus.SUM.
     */
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

void processor_t::debug_output_log(std::stringstream *s)
{
  if (log_file==stderr) {
    std::ostream out(sout_.rdbuf());
    out << s->str(); // handles command line options -d -s -l
  } else {
    fputs(s->str().c_str(), log_file); // handles command line option --log
  }
}

void processor_t::take_trap(trap_t& t, reg_t epc)
{
  if (debug) {
    std::stringstream s; // first put everything in a string, later send it to output
    s << "core " << std::dec << std::setfill(' ') << std::setw(3) << id
      << ": exception " << t.name() << ", epc 0x"
      << std::hex << std::setfill('0') << std::setw(max_xlen/4) << zext(epc, max_xlen) << std::endl;
    if (t.has_tval())
       s << "core " << std::dec << std::setfill(' ') << std::setw(3) << id
         << ":           tval 0x" << std::hex << std::setfill('0') << std::setw(max_xlen/4)
         << zext(t.get_tval(), max_xlen) << std::endl;
    debug_output_log(&s);
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
    vsdeleg = (curr_virt && state.prv <= PRV_S) ? (state.mideleg->read() & state.hideleg) : 0;
    hsdeleg = (state.prv <= PRV_S) ? state.mideleg->read() : 0;
    bit &= ~((reg_t)1 << (max_xlen-1));
  } else {
    vsdeleg = (curr_virt && state.prv <= PRV_S) ? (state.medeleg->read() & state.hedeleg) : 0;
    hsdeleg = (state.prv <= PRV_S) ? state.medeleg->read() : 0;
  }
  if (state.prv <= PRV_S && bit < max_xlen && ((vsdeleg >> bit) & 1)) {
    // Handle the trap in VS-mode
    reg_t vector = (state.vstvec->read() & 1) && interrupt ? 4*bit : 0;
    state.pc = (state.vstvec->read() & ~(reg_t)1) + vector;
    state.vscause->write((interrupt) ? (t.cause() - 1) : t.cause());
    state.vsepc->write(epc);
    state.vstval->write(t.get_tval());

    reg_t s = state.sstatus->read();
    s = set_field(s, MSTATUS_SPIE, get_field(s, MSTATUS_SIE));
    s = set_field(s, MSTATUS_SPP, state.prv);
    s = set_field(s, MSTATUS_SIE, 0);
    state.sstatus->write(s);
    set_privilege(PRV_S);
  } else if (state.prv <= PRV_S && bit < max_xlen && ((hsdeleg >> bit) & 1)) {
    // Handle the trap in HS-mode
    set_virt(false);
    reg_t vector = (state.stvec->read() & 1) && interrupt ? 4*bit : 0;
    state.pc = (state.stvec->read() & ~(reg_t)1) + vector;
    state.scause->write(t.cause());
    state.sepc->write(epc);
    state.stval->write(t.get_tval());
    state.htval = t.get_tval2();
    state.htinst = t.get_tinst();

    reg_t s = state.sstatus->read();
    s = set_field(s, MSTATUS_SPIE, get_field(s, MSTATUS_SIE));
    s = set_field(s, MSTATUS_SPP, state.prv);
    s = set_field(s, MSTATUS_SIE, 0);
    state.sstatus->write(s);
    if (extension_enabled('H')) {
      s = state.hstatus->read();
      if (curr_virt)
        s = set_field(s, HSTATUS_SPVP, state.prv);
      s = set_field(s, HSTATUS_SPV, curr_virt);
      s = set_field(s, HSTATUS_GVA, t.has_gva());
      state.hstatus->write(s);
    }
    set_privilege(PRV_S);
  } else {
    // Handle the trap in M-mode
    set_virt(false);
    reg_t vector = (state.mtvec->read() & 1) && interrupt ? 4*bit : 0;
    state.pc = (state.mtvec->read() & ~(reg_t)1) + vector;
    state.mepc->write(epc);
    state.mcause->write(t.cause());
    state.mtval->write(t.get_tval());
    state.mtval2 = t.get_tval2();
    state.mtinst = t.get_tinst();

    reg_t s = state.mstatus->read();
    s = set_field(s, MSTATUS_MPIE, get_field(s, MSTATUS_MIE));
    s = set_field(s, MSTATUS_MPP, state.prv);
    s = set_field(s, MSTATUS_MIE, 0);
    s = set_field(s, MSTATUS_MPV, curr_virt);
    s = set_field(s, MSTATUS_GVA, t.has_gva());
    state.mstatus->write(s);
    set_privilege(PRV_M);
  }
}

void processor_t::disasm(insn_t insn)
{
  uint64_t bits = insn.bits() & ((1ULL << (8 * insn_length(insn.bits()))) - 1);
  if (last_pc != state.pc || last_bits != bits) {
    std::stringstream s;  // first put everything in a string, later send it to output

#ifdef RISCV_ENABLE_COMMITLOG
    const char* sym = get_symbol(state.pc);
    if (sym != nullptr)
    {
      s << "core " << std::dec << std::setfill(' ') << std::setw(3) << id
        << ": >>>>  " << sym << std::endl;
    }
#endif

    if (executions != 1) {
      s << "core " << std::dec << std::setfill(' ') << std::setw(3) << id
        << ": Executed " << executions << " times" << std::endl;
    }

    s << "core " << std::dec << std::setfill(' ') << std::setw(3) << id
      << std::hex << ": 0x" << std::setfill('0') << std::setw(max_xlen/4)
      << zext(state.pc, max_xlen) << " (0x" << std::setw(8) << bits << ") "
      << disassembler->disassemble(insn) << std::endl;

    debug_output_log(&s);

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

void processor_t::set_csr(int which, reg_t val)
{
#if defined(RISCV_ENABLE_COMMITLOG)
#define LOG_CSR(rd) \
  STATE.log_reg_write[((which) << 4) | 4] = {get_csr(rd), 0};
#else
#define LOG_CSR(rd)
#endif

  val = zext_xlen(val);
  reg_t supervisor_ints = extension_enabled('S') ? MIP_SSIP | MIP_STIP | MIP_SEIP : 0;
  reg_t vssip_int = extension_enabled('H') ? MIP_VSSIP : 0;
  reg_t hypervisor_ints = extension_enabled('H') ? MIP_HS_MASK : 0;
  reg_t coprocessor_ints = (reg_t)any_custom_extensions() << IRQ_COP;
  reg_t delegable_ints = supervisor_ints | coprocessor_ints;
  reg_t all_ints = delegable_ints | hypervisor_ints | MIP_MSIP | MIP_MTIP | MIP_MEIP;
  auto search = state.csrmap.find(which);
  if (search != state.csrmap.end()) {
    search->second->write(val);
    return;
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
    case CSR_MTVAL2: state.mtval2 = val; break;
    case CSR_MTINST: state.mtinst = val; break;
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
    case CSR_HGEIE:
      /* Ignore */
      break;
    case CSR_HTVAL:
      state.htval = val;
      break;
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
      LOG_CSR(CSR_FFLAGS);
      break;
    case CSR_FRM:
      LOG_CSR(CSR_FRM);
      break;
    case CSR_FCSR:
      LOG_CSR(CSR_FFLAGS);
      LOG_CSR(CSR_FRM);
      LOG_CSR(CSR_FCSR);
      break;
    case CSR_VCSR:
      LOG_CSR(CSR_VXSAT);
      LOG_CSR(CSR_VXRM);
      break;

    case CSR_VSTART:
      LOG_CSR(CSR_VSTART);
      break;
    case CSR_VXSAT:
      LOG_CSR(CSR_VXSAT);
      break;
    case CSR_VXRM:
      LOG_CSR(CSR_VXRM);
      break;

    case CSR_MINSTRET:
    case CSR_MCYCLE:
    case CSR_MINSTRETH:
    case CSR_MCYCLEH:
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
#define mcounteren_ok(__which) \
({ \
  bool __ctr_ok = true; \
  if (state.prv < PRV_M) \
    __ctr_ok = (state.mcounteren->read() >> (__which & 31)) & 1;        \
  __ctr_ok; \
})
#define hcounteren_ok(__which) \
({ \
  bool __ctr_ok = true; \
  if (state.v) \
    __ctr_ok = (state.hcounteren->read() >> (__which & 31)) & 1;        \
  __ctr_ok; \
})
#define scounteren_ok(__which) \
({ \
  bool __ctr_ok = true; \
  if (extension_enabled('S') && state.prv < PRV_S) \
    __ctr_ok = (state.scounteren->read() >> (__which & 31)) & 1;        \
  __ctr_ok; \
})

  reg_t res = 0;
#define ret(n) do { \
    res = (n); \
    goto out; \
  } while (false)

  auto search = state.csrmap.find(which);
  if (search != state.csrmap.end()) {
    if (!peek)
      search->second->verify_permissions(insn, write);
    return search->second->read();
  }

  switch (which)
  {
    case CSR_SENTROPY:
      if (!extension_enabled(EXT_ZKR))
        break;
      /* Read-only access disallowed due to wipe-on-read side effect */
      if (!write)
        break;
      ret(es.get_sentropy());
    case CSR_FFLAGS:
      require_fp;
      if (!extension_enabled('F'))
        break;
      ret(state.fflags);
    case CSR_FRM:
      require_fp;
      if (!extension_enabled('F'))
        break;
      ret(state.frm);
    case CSR_FCSR:
      require_fp;
      if (!extension_enabled('F'))
        break;
      ret((state.fflags << FSR_AEXC_SHIFT) | (state.frm << FSR_RD_SHIFT));
    case CSR_VCSR:
      require_vector_vs;
      if (!extension_enabled('V'))
        break;
      ret((VU.vxsat << VCSR_VXSAT_SHIFT) | (VU.vxrm << VCSR_VXRM_SHIFT));
    case CSR_INSTRET:
    case CSR_CYCLE:
    case CSR_HPMCOUNTER3 ... CSR_HPMCOUNTER31:
      if (!mcounteren_ok(which))
          goto throw_illegal;
      if (!hcounteren_ok(which))
          goto throw_virtual;
      if (!scounteren_ok(which)) {
        if (state.v)
          goto throw_virtual;
        else
          goto throw_illegal;
      }
      if (which == CSR_INSTRET || which == CSR_CYCLE)
        ret(state.minstret);
      else
        ret(0);
    case CSR_MINSTRET:
    case CSR_MCYCLE:
    case CSR_MHPMCOUNTER3 ... CSR_MHPMCOUNTER31:
    case CSR_MHPMEVENT3 ... CSR_MHPMEVENT31:
      if (which == CSR_MINSTRET || which == CSR_MCYCLE)
        ret(state.minstret);
      else
        ret(0);
    case CSR_INSTRETH:
    case CSR_CYCLEH:
    case CSR_HPMCOUNTER3H ... CSR_HPMCOUNTER31H:
      if (!mcounteren_ok(which) || xlen != 32)
          goto throw_illegal;
      if (!hcounteren_ok(which))
          goto throw_virtual;
      if (!scounteren_ok(which)) {
        if (state.v)
          goto throw_virtual;
        else
          goto throw_illegal;
      }
      if (which == CSR_INSTRETH || which == CSR_CYCLEH)
        ret(state.minstret >> 32);
      else
        ret(0);
    case CSR_MINSTRETH:
    case CSR_MCYCLEH:
    case CSR_MHPMCOUNTER3H ... CSR_MHPMCOUNTER31H:
      if (xlen == 32) {
        if (which == CSR_MINSTRETH || which == CSR_MCYCLEH)
          ret(state.minstret >> 32);
        else
          ret(0);
      }
      break;
    case CSR_MCOUNTINHIBIT: ret(0);
    case CSR_MSTATUSH:
      if (xlen == 32)
        ret((state.mstatus->read() >> 32) & (MSTATUSH_SBE | MSTATUSH_MBE));
      break;
    case CSR_MTVAL2:
      if (extension_enabled('H'))
        ret(state.mtval2);
      break;
    case CSR_MTINST:
      if (extension_enabled('H'))
        ret(state.mtinst);
      break;
    case CSR_MARCHID: ret(5);
    case CSR_MIMPID: ret(0);
    case CSR_MVENDORID: ret(0);
    case CSR_MHARTID: ret(id);
    case CSR_HEDELEG: ret(state.hedeleg);
    case CSR_HIDELEG: ret(state.hideleg);
    case CSR_HGEIE: ret(0);
    case CSR_HTVAL: ret(state.htval);
    case CSR_HTINST: ret(state.htinst);
    case CSR_HGATP: {
      if (!state.v && get_field(state.mstatus->read(), MSTATUS_TVM))
        require_privilege(PRV_M);
      ret(state.hgatp);
    }
    case CSR_HGEIP: ret(0);
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
      if (!extension_enabled('V'))
        break;
      ret(VU.vstart);
    case CSR_VXSAT:
      require_vector_vs;
      if (!extension_enabled('V'))
        break;
      ret(VU.vxsat);
    case CSR_VXRM:
      require_vector_vs;
      if (!extension_enabled('V'))
        break;
      ret(VU.vxrm);
    case CSR_VL:
      require_vector_vs;
      if (!extension_enabled('V'))
        break;
      ret(VU.vl);
    case CSR_VTYPE:
      require_vector_vs;
      if (!extension_enabled('V'))
        break;
      ret(VU.vtype);
    case CSR_VLENB:
      require_vector_vs;
      if (!extension_enabled('V'))
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

  if ((csr_priv == PRV_S && !extension_enabled('S')) ||
      (csr_priv == PRV_HS && !extension_enabled('H')))
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
        bytes[0] = get_field(state.mip->read(), MIP_MSIP);
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
        state.mip->write_with_mask(MIP_MSIP, bytes[0] << IRQ_M_SOFT);
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
