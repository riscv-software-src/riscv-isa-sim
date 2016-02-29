// See LICENSE for license details.

#include "processor.h"
#include "extension.h"
#include "common.h"
#include "config.h"
#include "sim.h"
#include "htif.h"
#include "disasm.h"
#include <cinttypes>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <limits.h>
#include <stdexcept>
#include <algorithm>

#undef STATE
#define STATE state

processor_t::processor_t(const char* isa, sim_t* sim, uint32_t id)
  : sim(sim), ext(NULL), disassembler(new disassembler_t),
    id(id), run(false), debug(false)
{
  parse_isa_string(isa);

  mmu = new mmu_t(sim->mem, sim->memsz);
  mmu->set_processor(this);

  reset(true);

  register_base_instructions();
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

static void bad_isa_string(const char* isa)
{
  fprintf(stderr, "error: bad --isa option %s\n", isa);
  abort();
}

void processor_t::parse_isa_string(const char* str)
{
  std::string lowercase, tmp;
  for (const char *r = str; *r; r++)
    lowercase += std::tolower(*r);

  const char* p = lowercase.c_str();
  const char* all_subsets = "imafdc";

  max_xlen = 64;
  isa = reg_t(2) << 62;

  if (strncmp(p, "rv32", 4) == 0)
    max_xlen = 32, isa = 0, p += 4;
  else if (strncmp(p, "rv64", 4) == 0)
    p += 4;
  else if (strncmp(p, "rv", 2) == 0)
    p += 2;

  if (!*p) {
    p = all_subsets;
  } else if (*p == 'g') { // treat "G" as "IMAFD"
    tmp = std::string("imafd") + (p+1);
    p = &tmp[0];
  } else if (*p != 'i') {
    bad_isa_string(str);
  }

  isa_string = "rv" + std::to_string(max_xlen) + p;
  isa |= 1L << ('s' - 'a'); // advertise support for supervisor mode

  while (*p) {
    isa |= 1L << (*p - 'a');

    if (auto next = strchr(all_subsets, *p)) {
      all_subsets = next + 1;
      p++;
    } else if (*p == 'x') {
      const char* ext = p+1, *end = ext;
      while (islower(*end))
        end++;
      register_extension(find_extension(std::string(ext, end - ext).c_str())());
      p = end;
    } else {
      bad_isa_string(str);
    }
  }

  if (supports_extension('D') && !supports_extension('F'))
    bad_isa_string(str);

  // if we have IMAFD, advertise G, too
  if (supports_extension('I') && supports_extension('M') &&
      supports_extension('A') && supports_extension('D'))
    isa |= 1L << ('g' - 'a');

  // advertise support for supervisor and user modes
  isa |= 1L << ('s' - 'a');
  isa |= 1L << ('u' - 'a');
}

void state_t::reset()
{
  memset(this, 0, sizeof(*this));
  prv = PRV_M;
  pc = DEFAULT_RSTVEC;
  load_reservation = -1;
}

void processor_t::set_debug(bool value)
{
  debug = value;
  if (ext)
    ext->set_debug(value);
}

void processor_t::set_histogram(bool value)
{
  histogram_enabled = value;
#ifndef RISCV_ENABLE_HISTOGRAM
  if (value) {
    fprintf(stderr, "PC Histogram support has not been properly enabled;");
    fprintf(stderr, " please re-build the riscv-isa-run project using \"configure --enable-histogram\".\n");
  }
#endif
}

void processor_t::reset(bool value)
{
  if (run == !value)
    return;
  run = !value;

  state.reset();
  set_csr(CSR_MSTATUS, state.mstatus);

  if (ext)
    ext->reset(); // reset the extension
}

void processor_t::raise_interrupt(reg_t which)
{
  throw trap_t(((reg_t)1 << (max_xlen-1)) | which);
}

static int ctz(reg_t val)
{
  int res = 0;
  if (val)
    while ((val & 1) == 0)
      val >>= 1, res++;
  return res;
}

void processor_t::take_interrupt()
{
  check_timer();

  reg_t interrupts = state.mip & state.mie;

  reg_t m_interrupts = interrupts & ~state.mideleg;
  reg_t mie = get_field(state.mstatus, MSTATUS_MIE);
  if ((state.prv < PRV_M || (state.prv == PRV_M && mie)) && m_interrupts)
    raise_interrupt(ctz(m_interrupts));

  reg_t s_interrupts = interrupts & state.mideleg;
  reg_t sie = get_field(state.mstatus, MSTATUS_SIE);
  if ((state.prv < PRV_S || (state.prv == PRV_S && sie)) && s_interrupts)
    raise_interrupt(ctz(s_interrupts));
}

void processor_t::check_timer()
{
  if (sim->rtc >= state.mtimecmp)
    state.mip |= MIP_MTIP;
}

static bool validate_priv(reg_t priv)
{
  return priv == PRV_U || priv == PRV_S || priv == PRV_M;
}

void processor_t::set_privilege(reg_t prv)
{
  assert(validate_priv(prv));
  mmu->flush_tlb();
  state.prv = prv;
}

void processor_t::take_trap(trap_t& t, reg_t epc)
{
  if (debug)
    fprintf(stderr, "core %3d: exception %s, epc 0x%016" PRIx64 "\n",
            id, t.name(), epc);

  // by default, trap to M-mode, unless delegated to S-mode
  reg_t bit = t.cause();
  reg_t deleg = state.medeleg;
  if (bit & ((reg_t)1 << (max_xlen-1)))
    deleg = state.mideleg, bit &= ~((reg_t)1 << (max_xlen-1));
  if (state.prv <= PRV_S && bit < max_xlen && ((deleg >> bit) & 1)) {
    // handle the trap in S-mode
    state.pc = state.stvec;
    state.scause = t.cause();
    state.sepc = epc;
    if (t.has_badaddr())
      state.sbadaddr = t.get_badaddr();

    reg_t s = state.mstatus;
    s = set_field(s, MSTATUS_SPIE, get_field(s, MSTATUS_UIE << state.prv));
    s = set_field(s, MSTATUS_SPP, state.prv);
    s = set_field(s, MSTATUS_SIE, 0);
    set_csr(CSR_MSTATUS, s);
    set_privilege(PRV_S);
  } else {
    state.pc = DEFAULT_MTVEC;
    state.mcause = t.cause();
    state.mepc = epc;
    if (t.has_badaddr())
      state.mbadaddr = t.get_badaddr();

    reg_t s = state.mstatus;
    s = set_field(s, MSTATUS_MPIE, get_field(s, MSTATUS_UIE << state.prv));
    s = set_field(s, MSTATUS_MPP, state.prv);
    s = set_field(s, MSTATUS_MIE, 0);
    set_csr(CSR_MSTATUS, s);
    set_privilege(PRV_M);
  }

  yield_load_reservation();
}

void processor_t::disasm(insn_t insn)
{
  uint64_t bits = insn.bits() & ((1ULL << (8 * insn_length(insn.bits()))) - 1);
  fprintf(stderr, "core %3d: 0x%016" PRIx64 " (0x%08" PRIx64 ") %s\n",
          id, state.pc, bits, disassembler->disassemble(insn).c_str());
}

static bool validate_vm(int max_xlen, reg_t vm)
{
  if (max_xlen == 64 && (vm == VM_SV39 || vm == VM_SV48))
    return true;
  if (max_xlen == 32 && vm == VM_SV32)
    return true;
  return vm == VM_MBARE;
}

void processor_t::set_csr(int which, reg_t val)
{
  val = zext_xlen(val);
  reg_t all_ints = MIP_SSIP | MIP_MSIP | MIP_STIP | MIP_MTIP | (1UL << IRQ_HOST);
  reg_t s_ints = MIP_SSIP | MIP_STIP;
  switch (which)
  {
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
    case CSR_MSTATUS: {
      if ((val ^ state.mstatus) &
          (MSTATUS_VM | MSTATUS_MPP | MSTATUS_MPRV | MSTATUS_PUM))
        mmu->flush_tlb();

      reg_t mask = MSTATUS_SIE | MSTATUS_SPIE | MSTATUS_MIE | MSTATUS_MPIE
                 | MSTATUS_SPP | MSTATUS_FS | MSTATUS_MPRV | MSTATUS_PUM
                 | (ext ? MSTATUS_XS : 0);

      if (validate_vm(max_xlen, get_field(val, MSTATUS_VM)))
        mask |= MSTATUS_VM;
      if (validate_priv(get_field(val, MSTATUS_MPP)))
        mask |= MSTATUS_MPP;

      state.mstatus = (state.mstatus & ~mask) | (val & mask);

      bool dirty = (state.mstatus & MSTATUS_FS) == MSTATUS_FS;
      dirty |= (state.mstatus & MSTATUS_XS) == MSTATUS_XS;
      if (max_xlen == 32)
        state.mstatus = set_field(state.mstatus, MSTATUS32_SD, dirty);
      else
        state.mstatus = set_field(state.mstatus, MSTATUS64_SD, dirty);

      // spike supports the notion of xlen < max_xlen, but current priv spec
      // doesn't provide a mechanism to run RV32 software on an RV64 machine
      xlen = max_xlen;
      break;
    }
    case CSR_MIP: {
      reg_t mask = all_ints &~ MIP_MTIP;
      state.mip = (state.mip & ~mask) | (val & mask);
      break;
    }
    case CSR_MIPI:
      state.mip = set_field(state.mip, MIP_MSIP, val & 1);
      break;
    case CSR_MIE:
      state.mie = (state.mie & ~all_ints) | (val & all_ints);
      break;
    case CSR_MIDELEG:
      state.mideleg = (state.mideleg & ~s_ints) | (val & s_ints);
      break;
    case CSR_MEDELEG: {
      reg_t mask = 0;
#define DECLARE_CAUSE(name, value) mask |= 1ULL << (value);
#include "encoding.h"
#undef DECLARE_CAUSE
      state.medeleg = (state.medeleg & ~mask) | (val & mask);
      break;
    }
    case CSR_SSTATUS: {
      reg_t mask = SSTATUS_SIE | SSTATUS_SPIE | SSTATUS_SPP | SSTATUS_FS
                 | SSTATUS_XS | SSTATUS_PUM;
      set_csr(CSR_MSTATUS, (state.mstatus & ~mask) | (val & mask));
      break;
    }
    case CSR_SIP: {
      reg_t mask = s_ints &~ MIP_STIP;
      state.mip = (state.mip & ~mask) | (val & mask);
      break;
    }
    case CSR_SIE: {
      reg_t mask = s_ints;
      state.mie = (state.mie & ~mask) | (val & mask);
      break;
    }
    case CSR_SEPC: state.sepc = val; break;
    case CSR_STVEC: state.stvec = val >> 2 << 2; break;
    case CSR_SPTBR: state.sptbr = val; break;
    case CSR_SSCRATCH: state.sscratch = val; break;
    case CSR_SCAUSE: state.scause = val; break;
    case CSR_SBADADDR: state.sbadaddr = val; break;
    case CSR_MEPC: state.mepc = val; break;
    case CSR_MSCRATCH: state.mscratch = val; break;
    case CSR_MCAUSE: state.mcause = val; break;
    case CSR_MBADADDR: state.mbadaddr = val; break;
    case CSR_MTIMECMP:
      state.mip &= ~MIP_MTIP;
      state.mtimecmp = val;
      break;
    case CSR_MTOHOST:
      if (state.tohost == 0)
        state.tohost = val;
      break;
    case CSR_MFROMHOST:
      state.mip = (state.mip & ~(1 << IRQ_HOST)) | (val ? (1 << IRQ_HOST) : 0);
      state.fromhost = val;
      break;
  }
}

reg_t processor_t::get_csr(int which)
{
  switch (which)
  {
    case CSR_FFLAGS:
      require_fp;
      if (!supports_extension('F'))
        break;
      return state.fflags;
    case CSR_FRM:
      require_fp;
      if (!supports_extension('F'))
        break;
      return state.frm;
    case CSR_FCSR:
      require_fp;
      if (!supports_extension('F'))
        break;
      return (state.fflags << FSR_AEXC_SHIFT) | (state.frm << FSR_RD_SHIFT);
    case CSR_MTIME: return sim->rtc;
    case CSR_MCYCLE: return state.minstret;
    case CSR_MINSTRET: return state.minstret;
    case CSR_MTIMEH: if (xlen > 32) break; else return sim->rtc >> 32;
    case CSR_MCYCLEH: if (xlen > 32) break; else return state.minstret >> 32;
    case CSR_MINSTRETH: if (xlen > 32) break; else return state.minstret >> 32;
    case CSR_SSTATUS: {
      reg_t mask = SSTATUS_SIE | SSTATUS_SPIE | SSTATUS_SPP | SSTATUS_FS
                 | SSTATUS_XS | SSTATUS_PUM;
      reg_t sstatus = state.mstatus & mask;
      if ((sstatus & SSTATUS_FS) == SSTATUS_FS ||
          (sstatus & SSTATUS_XS) == SSTATUS_XS)
        sstatus |= (xlen == 32 ? SSTATUS32_SD : SSTATUS64_SD);
      return sstatus;
    }
    case CSR_SIP: return state.mip & (MIP_SSIP | MIP_STIP);
    case CSR_SIE: return state.mie & (MIP_SSIP | MIP_STIP);
    case CSR_SEPC: return state.sepc;
    case CSR_SBADADDR: return state.sbadaddr;
    case CSR_STVEC: return state.stvec;
    case CSR_SCAUSE:
      if (max_xlen > xlen)
        return state.scause | ((state.scause >> (max_xlen-1)) << (xlen-1));
      return state.scause;
    case CSR_SPTBR: return state.sptbr;
    case CSR_SASID: return 0;
    case CSR_SSCRATCH: return state.sscratch;
    case CSR_MSTATUS: return state.mstatus;
    case CSR_MIP: return state.mip;
    case CSR_MIPI: return 0;
    case CSR_MIE: return state.mie;
    case CSR_MEPC: return state.mepc;
    case CSR_MSCRATCH: return state.mscratch;
    case CSR_MCAUSE: return state.mcause;
    case CSR_MBADADDR: return state.mbadaddr;
    case CSR_MTIMECMP: return state.mtimecmp;
    case CSR_MISA: return isa;
    case CSR_MARCHID: return 0;
    case CSR_MIMPID: return 0;
    case CSR_MVENDORID: return 0;
    case CSR_MHARTID: return id;
    case CSR_MTVEC: return DEFAULT_MTVEC;
    case CSR_MEDELEG: return state.medeleg;
    case CSR_MIDELEG: return state.mideleg;
    case CSR_MTOHOST:
      sim->get_htif()->tick(); // not necessary, but faster
      return state.tohost;
    case CSR_MFROMHOST:
      sim->get_htif()->tick(); // not necessary, but faster
      return state.fromhost;
    case CSR_MCFGADDR: return sim->memsz;
    case CSR_UARCH0:
    case CSR_UARCH1:
    case CSR_UARCH2:
    case CSR_UARCH3:
    case CSR_UARCH4:
    case CSR_UARCH5:
    case CSR_UARCH6:
    case CSR_UARCH7:
    case CSR_UARCH8:
    case CSR_UARCH9:
    case CSR_UARCH10:
    case CSR_UARCH11:
    case CSR_UARCH12:
    case CSR_UARCH13:
    case CSR_UARCH14:
    case CSR_UARCH15:
      return 0;
  }
  throw trap_illegal_instruction();
}

reg_t illegal_instruction(processor_t* p, insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction();
}

insn_func_t processor_t::decode_insn(insn_t insn)
{
  // look up opcode in hash table
  size_t idx = insn.bits() % OPCODE_CACHE_SIZE;
  insn_desc_t desc = opcode_cache[idx];

  if (unlikely(insn.bits() != desc.match)) {
    // fall back to linear search
    insn_desc_t* p = &instructions[0];
    while ((insn.bits() & p->mask) != p->match)
      p++;
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
    opcode_cache[i] = {1, 0, &illegal_instruction, &illegal_instruction};
}

void processor_t::register_extension(extension_t* x)
{
  for (auto insn : x->get_instructions())
    register_insn(insn);
  build_opcode_map();
  for (auto disasm_insn : x->get_disasms())
    disassembler->add_insn(disasm_insn);
  if (ext != NULL)
    throw std::logic_error("only one extension may be registered");
  ext = x;
  x->set_processor(this);
}

void processor_t::register_base_instructions()
{
  #define DECLARE_INSN(name, match, mask) \
    insn_bits_t name##_match = (match), name##_mask = (mask);
  #include "encoding.h"
  #undef DECLARE_INSN

  #define DEFINE_INSN(name) \
    REGISTER_INSN(this, name, name##_match, name##_mask)
  #include "insn_list.h"
  #undef DEFINE_INSN

  register_insn({0, 0, &illegal_instruction, &illegal_instruction});
  build_opcode_map();
}

bool processor_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  try {
    auto res = get_csr(addr / (max_xlen / 8));
    memcpy(bytes, &res, len);
    return true;
  } catch (trap_illegal_instruction& t) {
    return false;
  }
}

bool processor_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  try {
    reg_t value = 0;
    memcpy(&value, bytes, len);
    set_csr(addr / (max_xlen / 8), value);
    return true;
  } catch (trap_illegal_instruction& t) {
    return false;
  }
}
