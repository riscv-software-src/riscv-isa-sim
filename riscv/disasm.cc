// See LICENSE for license details.

#include "disasm.h"
#include <string>
#include <vector>
#include <cstdarg>
#include <sstream>
#include <stdlib.h>
using namespace std;

class arg_t
{
 public:
  virtual string to_string(insn_t val) const = 0;
  virtual ~arg_t() {}
};

static const char* xpr[] = {
  "zero", "ra", "s0", "s1",  "s2",  "s3",  "s4",  "s5",
  "s6",   "s7", "s8", "s9", "s10", "s11",  "sp",  "tp",
  "v0",   "v1", "a0", "a1",  "a2",  "a3",  "a4",  "a5",
  "a6",   "a7", "t0", "t1",  "t2",  "t3",  "t4",  "gp"
};

static const char* fpr[] = {
  "fs0", "fs1",  "fs2",  "fs3",  "fs4",  "fs5",  "fs6",  "fs7",
  "fs8", "fs9", "fs10", "fs11", "fs12", "fs13", "fs14", "fs15",
  "fv0", "fv1", "fa0",   "fa1",  "fa2",  "fa3",  "fa4",  "fa5",
  "fa6", "fa7", "ft0",   "ft1",  "ft2",  "ft3",  "ft4",  "ft5"
};

static const char* vxpr[] = {
  "vx0",  "vx1",  "vx2",  "vx3",  "vx4",  "vx5",  "vx6",  "vx7",
  "vx8",  "vx9",  "vx10", "vx11", "vx12", "vx13", "vx14", "vx15",
  "vx16", "vx17", "vx18", "vx19", "vx20", "vx21", "vx22", "vx23",
  "vx24", "vx25", "vx26", "vx27", "vx28", "vx29", "vx30", "vx31"
};

static const char* vfpr[] = {
  "vf0",  "vf1",  "vf2",  "vf3",  "vf4",  "vf5",  "vf6",  "vf7",
  "vf8",  "vf9",  "vf10", "vf11", "vf12", "vf13", "vf14", "vf15",
  "vf16", "vf17", "vf18", "vf19", "vf20", "vf21", "vf22", "vf23",
  "vf24", "vf25", "vf26", "vf27", "vf28", "vf29", "vf30", "vf31"
};

struct : public arg_t {
  string to_string(insn_t insn) const {
    return ::to_string((int)insn.i_imm()) + '(' + xpr[insn.rs1()] + ')';
  }
} load_address;

struct : public arg_t {
  string to_string(insn_t insn) const {
    return ::to_string((int)insn.s_imm()) + '(' + xpr[insn.rs1()] + ')';
  }
} store_address;

struct : public arg_t {
  string to_string(insn_t insn) const {
    return string("0(") + xpr[insn.rs1()] + ')';
  }
} amo_address;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return xpr[insn.rd()];
  }
} xrd;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return xpr[insn.rs1()];
  }
} xrs1;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return xpr[insn.rs2()];
  }
} xrs2;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return fpr[insn.rd()];
  }
} frd;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return fpr[insn.rs1()];
  }
} frs1;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return fpr[insn.rs2()];
  }
} frs2;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return fpr[insn.rs3()];
  }
} frs3;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return vxpr[insn.rd()];
  }
} vxrd;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return vxpr[insn.rs1()];
  }
} vxrs1;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return vfpr[insn.rd()];
  }
} vfrd;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return vfpr[insn.rs1()];
  }
} vfrs1;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return ::to_string(insn.i_imm() & 0x3f);
  }
} nxregs;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return ::to_string((insn.i_imm() >> 6) & 0x3f);
  }
} nfregs;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return string("pcr") + xpr[insn.rs1()];
  }
} pcr;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return ::to_string((int)insn.i_imm());
  }
} imm;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    std::stringstream s;
    s << std::hex << "0x" << ((uint32_t)insn.u_imm() >> 12);
    return s.str();
  }
} bigimm;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    std::stringstream s;
    int32_t target = insn.sb_imm();
    char sign = target >= 0 ? '+' : '-';
    s << "pc " << sign << ' ' << abs(target);
    return s.str();
  }
} branch_target;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    std::stringstream s;
    int32_t target = insn.sb_imm();
    char sign = target >= 0 ? '+' : '-';
    s << "pc " << sign << std::hex << " 0x" << abs(target);
    return s.str();
  }
} jump_target;

class disasm_insn_t
{
 public:
  disasm_insn_t(const char* name, uint32_t match, uint32_t mask,
                const std::vector<const arg_t*>& args)
    : match(match), mask(mask), args(args), name(name) {}

  bool operator == (insn_t insn) const
  {
    return (insn.bits() & mask) == match;
  }

  std::string to_string(insn_t insn) const
  {
    std::stringstream s;
    int len;
    for (len = 0; name[len]; len++)
      s << (name[len] == '_' ? '.' : name[len]);

    if (args.size())
    {
      s << std::string(std::max(1, 8 - len), ' ');
      for (size_t i = 0; i < args.size()-1; i++)
        s << args[i]->to_string(insn) << ", ";
      s << args[args.size()-1]->to_string(insn);
    }
    return s.str();
  }

  uint32_t get_match() const { return match; }
  uint32_t get_mask() const { return mask; }

 private:
  uint32_t match;
  uint32_t mask;
  std::vector<const arg_t*> args;
  const char* name;
};

std::string disassembler::disassemble(insn_t insn)
{
  const disasm_insn_t* disasm_insn = lookup(insn);
  return disasm_insn ? disasm_insn->to_string(insn) : "unknown";
}

disassembler::disassembler()
{
  const uint32_t mask_rd = 0x1fUL << 27;
  const uint32_t match_rd_ra = 1UL << 27;
  const uint32_t mask_rs1 = 0x1fUL << 22;
  const uint32_t match_rs1_ra = 1UL << 22;
  const uint32_t mask_rs2 = 0x1fUL << 17;
  const uint32_t mask_imm = 0xfffUL << 10;

  #define DECLARE_INSN(code, match, mask) \
   const uint32_t match_##code = match; \
   const uint32_t mask_##code = mask;
  #include "opcodes.h"
  #undef DECLARE_INSN

  // explicit per-instruction disassembly
  #define DISASM_INSN(name, code, extra, ...) \
    add_insn(new disasm_insn_t(name, match_##code, mask_##code | (extra), __VA_ARGS__));
  #define DEFINE_NOARG(code) \
    add_insn(new disasm_insn_t(#code, match_##code, mask_##code, {}));
  #define DEFINE_DTYPE(code) DISASM_INSN(#code, code, 0, {&xrd})
  #define DEFINE_RTYPE(code) DISASM_INSN(#code, code, 0, {&xrd, &xrs1, &xrs2})
  #define DEFINE_ITYPE(code) DISASM_INSN(#code, code, 0, {&xrd, &xrs1, &imm})
  #define DEFINE_I0TYPE(name, code) DISASM_INSN(name, code, mask_rs1, {&xrd, &imm})
  #define DEFINE_I1TYPE(name, code) DISASM_INSN(name, code, mask_imm, {&xrd, &xrs1})
  #define DEFINE_I2TYPE(name, code) DISASM_INSN(name, code, mask_rd | mask_imm, {&xrs1})
  #define DEFINE_LTYPE(code) DISASM_INSN(#code, code, 0, {&xrd, &bigimm})
  #define DEFINE_BTYPE(code) DISASM_INSN(#code, code, 0, {&xrs1, &xrs2, &branch_target})
  #define DEFINE_B0TYPE(name, code) DISASM_INSN(name, code, mask_rs1 | mask_rs2, {&branch_target})
  #define DEFINE_B1TYPE(name, code) DISASM_INSN(name, code, mask_rs2, {&xrs1, &branch_target})
  #define DEFINE_XLOAD(code) DISASM_INSN(#code, code, 0, {&xrd, &load_address})
  #define DEFINE_XSTORE(code) DISASM_INSN(#code, code, 0, {&xrs2, &store_address})
  #define DEFINE_XAMO(code) DISASM_INSN(#code, code, 0, {&xrd, &xrs2, &amo_address})
  #define DEFINE_FLOAD(code) DISASM_INSN(#code, code, 0, {&frd, &load_address})
  #define DEFINE_FSTORE(code) DISASM_INSN(#code, code, 0, {&frs2, &store_address})
  #define DEFINE_FRTYPE(code) DISASM_INSN(#code, code, 0, {&frd, &frs1, &frs2})
  #define DEFINE_FR1TYPE(code) DISASM_INSN(#code, code, 0, {&frd, &frs1})
  #define DEFINE_FR3TYPE(code) DISASM_INSN(#code, code, 0, {&frd, &frs1, &frs2, &frs3})
  #define DEFINE_FXTYPE(code) DISASM_INSN(#code, code, 0, {&xrd, &frs1})
  #define DEFINE_XFTYPE(code) DISASM_INSN(#code, code, 0, {&frd, &xrs1})

  DEFINE_XLOAD(lb)
  DEFINE_XLOAD(lbu)
  DEFINE_XLOAD(lh)
  DEFINE_XLOAD(lhu)
  DEFINE_XLOAD(lw)
  DEFINE_XLOAD(lwu)
  DEFINE_XLOAD(ld)

  DEFINE_XSTORE(sb)
  DEFINE_XSTORE(sh)
  DEFINE_XSTORE(sw)
  DEFINE_XSTORE(sd)

  DEFINE_XAMO(amoadd_w)
  DEFINE_XAMO(amoswap_w)
  DEFINE_XAMO(amoand_w)
  DEFINE_XAMO(amoor_w)
  DEFINE_XAMO(amomin_w)
  DEFINE_XAMO(amomax_w)
  DEFINE_XAMO(amominu_w)
  DEFINE_XAMO(amomaxu_w)
  DEFINE_XAMO(amoadd_d)
  DEFINE_XAMO(amoswap_d)
  DEFINE_XAMO(amoand_d)
  DEFINE_XAMO(amoor_d)
  DEFINE_XAMO(amomin_d)
  DEFINE_XAMO(amomax_d)
  DEFINE_XAMO(amominu_d)
  DEFINE_XAMO(amomaxu_d)

  DEFINE_XAMO(lr_w)
  DEFINE_XAMO(sc_w)
  DEFINE_XAMO(lr_d)
  DEFINE_XAMO(sc_d)

  DEFINE_FLOAD(flw)
  DEFINE_FLOAD(fld)

  DEFINE_FSTORE(fsw)
  DEFINE_FSTORE(fsd)

  add_insn(new disasm_insn_t("j", match_jal, mask_jal | mask_rd, {&jump_target}));
  add_insn(new disasm_insn_t("jal", match_jal | match_rd_ra, mask_jal | mask_rd, {&jump_target}));
  add_insn(new disasm_insn_t("jal", match_jal, mask_jal, {&xrd, &jump_target}));

  DEFINE_B0TYPE("b",    beq);
  DEFINE_B1TYPE("beqz", beq);
  DEFINE_B1TYPE("bnez", bne);
  DEFINE_B1TYPE("bltz", blt);
  DEFINE_B1TYPE("bgez", bge);
  DEFINE_BTYPE(beq)
  DEFINE_BTYPE(bne)
  DEFINE_BTYPE(blt)
  DEFINE_BTYPE(bge)
  DEFINE_BTYPE(bltu)
  DEFINE_BTYPE(bgeu)

  DEFINE_LTYPE(lui);
  DEFINE_LTYPE(auipc);

  DEFINE_I2TYPE("jr", jalr);
  add_insn(new disasm_insn_t("jalr", match_jalr | match_rd_ra, mask_jalr | mask_rd | mask_imm, {&xrs1}));
  add_insn(new disasm_insn_t("ret", match_jalr | match_rs1_ra, mask_jalr | mask_rd | mask_rs1 | mask_imm, {}));
  DEFINE_ITYPE(jalr);

  add_insn(new disasm_insn_t("nop", match_addi, mask_addi | mask_rd | mask_rs1 | mask_imm, {}));
  DEFINE_I0TYPE("li", addi);
  DEFINE_I1TYPE("move", addi);
  DEFINE_ITYPE(addi);
  DEFINE_ITYPE(slli);
  DEFINE_ITYPE(slti);
  DEFINE_ITYPE(sltiu);
  DEFINE_ITYPE(xori);
  DEFINE_ITYPE(srli);
  DEFINE_ITYPE(srai);
  DEFINE_ITYPE(ori);
  DEFINE_ITYPE(andi);
  DEFINE_ITYPE(addiw);
  DEFINE_ITYPE(slliw);
  DEFINE_ITYPE(srliw);
  DEFINE_ITYPE(sraiw);

  DEFINE_RTYPE(add);
  DEFINE_RTYPE(sub);
  DEFINE_RTYPE(sll);
  DEFINE_RTYPE(slt);
  DEFINE_RTYPE(sltu);
  DEFINE_RTYPE(xor);
  DEFINE_RTYPE(srl);
  DEFINE_RTYPE(sra);
  DEFINE_RTYPE(or);
  DEFINE_RTYPE(and);
  DEFINE_RTYPE(mul);
  DEFINE_RTYPE(mulh);
  DEFINE_RTYPE(mulhu);
  DEFINE_RTYPE(mulhsu);
  DEFINE_RTYPE(div);
  DEFINE_RTYPE(divu);
  DEFINE_RTYPE(rem);
  DEFINE_RTYPE(remu);
  DEFINE_RTYPE(addw);
  DEFINE_RTYPE(subw);
  DEFINE_RTYPE(sllw);
  DEFINE_RTYPE(srlw);
  DEFINE_RTYPE(sraw);
  DEFINE_RTYPE(mulw);
  DEFINE_RTYPE(divw);
  DEFINE_RTYPE(divuw);
  DEFINE_RTYPE(remw);
  DEFINE_RTYPE(remuw);

  DEFINE_NOARG(syscall);
  DEFINE_NOARG(break);
  DEFINE_NOARG(fence);
  DEFINE_NOARG(fence_i);

  DEFINE_DTYPE(rdcycle);
  DEFINE_DTYPE(rdtime);
  DEFINE_DTYPE(rdinstret);

  add_insn(new disasm_insn_t("mtpcr", match_mtpcr, mask_mtpcr | mask_rd, {&xrs2, &pcr}));
  add_insn(new disasm_insn_t("mtpcr", match_mtpcr, mask_mtpcr, {&xrd, &xrs2, &pcr}));
  add_insn(new disasm_insn_t("mfpcr", match_mfpcr, mask_mfpcr, {&xrd, &pcr}));
  add_insn(new disasm_insn_t("setpcr", match_setpcr, mask_setpcr, {&xrd, &pcr, &imm}));
  add_insn(new disasm_insn_t("clearpcr", match_clearpcr, mask_clearpcr, {&xrd, &pcr, &imm}));
  DEFINE_NOARG(eret)

  DEFINE_FRTYPE(fadd_s);
  DEFINE_FRTYPE(fsub_s);
  DEFINE_FRTYPE(fmul_s);
  DEFINE_FRTYPE(fdiv_s);
  DEFINE_FR1TYPE(fsqrt_s);
  DEFINE_FRTYPE(fmin_s);
  DEFINE_FRTYPE(fmax_s);
  DEFINE_FR3TYPE(fmadd_s);
  DEFINE_FR3TYPE(fmsub_s);
  DEFINE_FR3TYPE(fnmadd_s);
  DEFINE_FR3TYPE(fnmsub_s);
  DEFINE_FRTYPE(fsgnj_s);
  DEFINE_FRTYPE(fsgnjn_s);
  DEFINE_FRTYPE(fsgnjx_s);
  DEFINE_FR1TYPE(fcvt_s_d);
  DEFINE_XFTYPE(fcvt_s_l);
  DEFINE_XFTYPE(fcvt_s_lu);
  DEFINE_XFTYPE(fcvt_s_w);
  DEFINE_XFTYPE(fcvt_s_wu);
  DEFINE_XFTYPE(fcvt_s_wu);
  DEFINE_XFTYPE(fmv_s_x);
  DEFINE_FXTYPE(fcvt_l_s);
  DEFINE_FXTYPE(fcvt_lu_s);
  DEFINE_FXTYPE(fcvt_w_s);
  DEFINE_FXTYPE(fcvt_wu_s);
  DEFINE_FXTYPE(fmv_x_s);
  DEFINE_FXTYPE(feq_s);
  DEFINE_FXTYPE(flt_s);
  DEFINE_FXTYPE(fle_s);

  DEFINE_FRTYPE(fadd_d);
  DEFINE_FRTYPE(fsub_d);
  DEFINE_FRTYPE(fmul_d);
  DEFINE_FRTYPE(fdiv_d);
  DEFINE_FR1TYPE(fsqrt_d);
  DEFINE_FRTYPE(fmin_d);
  DEFINE_FRTYPE(fmax_d);
  DEFINE_FR3TYPE(fmadd_d);
  DEFINE_FR3TYPE(fmsub_d);
  DEFINE_FR3TYPE(fnmadd_d);
  DEFINE_FR3TYPE(fnmsub_d);
  DEFINE_FRTYPE(fsgnj_d);
  DEFINE_FRTYPE(fsgnjn_d);
  DEFINE_FRTYPE(fsgnjx_d);
  DEFINE_FR1TYPE(fcvt_d_s);
  DEFINE_XFTYPE(fcvt_d_l);
  DEFINE_XFTYPE(fcvt_d_lu);
  DEFINE_XFTYPE(fcvt_d_w);
  DEFINE_XFTYPE(fcvt_d_wu);
  DEFINE_XFTYPE(fcvt_d_wu);
  DEFINE_XFTYPE(fmv_d_x);
  DEFINE_FXTYPE(fcvt_l_d);
  DEFINE_FXTYPE(fcvt_lu_d);
  DEFINE_FXTYPE(fcvt_w_d);
  DEFINE_FXTYPE(fcvt_wu_d);
  DEFINE_FXTYPE(fmv_x_d);
  DEFINE_FXTYPE(feq_d);
  DEFINE_FXTYPE(flt_d);
  DEFINE_FXTYPE(fle_d);

  add_insn(new disasm_insn_t("fssr", match_fssr, mask_fssr | mask_rd, {&xrs1}));
  add_insn(new disasm_insn_t("fssr", match_fssr, mask_fssr, {&xrd, &xrs1}));
  DEFINE_DTYPE(frsr);

  // provide a default disassembly for all instructions as a fallback
  #define DECLARE_INSN(code, match, mask) \
   add_insn(new disasm_insn_t(#code " (args unknown)", match, mask, {}));
  #include "opcodes.h"
  #undef DECLARE_INSN
}

const disasm_insn_t* disassembler::lookup(insn_t insn)
{
  size_t idx = insn.bits() % HASH_SIZE;
  for (size_t j = 0; j < chain[idx].size(); j++)
    if(*chain[idx][j] == insn)
      return chain[idx][j];

  idx = HASH_SIZE;
  for (size_t j = 0; j < chain[idx].size(); j++)
    if(*chain[idx][j] == insn)
      return chain[idx][j];

  return NULL;
}

void disassembler::add_insn(disasm_insn_t* insn)
{
  size_t idx = HASH_SIZE;
  if (insn->get_mask() % HASH_SIZE == HASH_SIZE - 1)
    idx = insn->get_match() % HASH_SIZE;
  chain[idx].push_back(insn);
}

disassembler::~disassembler()
{
  for (size_t i = 0; i < HASH_SIZE+1; i++)
    for (size_t j = 0; j < chain[i].size(); j++)
      delete chain[i][j];
}
