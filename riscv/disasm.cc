// See LICENSE for license details.

#include "disasm.h"
#include <string>
#include <vector>
#include <cstdarg>
#include <sstream>
#include <stdlib.h>

class arg_t
{
 public:
  virtual std::string to_string(insn_t val) const = 0;
  virtual ~arg_t() {}
};

static const char* xpr_to_string[] = {
  "zero", "ra", "s0", "s1",  "s2",  "s3",  "s4",  "s5",
  "s6",   "s7", "s8", "s9", "s10", "s11",  "sp",  "tp",
  "v0",   "v1", "a0", "a1",  "a2",  "a3",  "a4",  "a5",
  "a6",   "a7", "a8", "a9", "a10", "a11", "a12", "a13"
};

static const char* fpr_to_string[] = {
  "fs0", "fs1",  "fs2",  "fs3",  "fs4",  "fs5",  "fs6",  "fs7",
  "fs8", "fs9", "fs10", "fs11", "fs12", "fs13", "fs14", "fs15",
  "fv0", "fv1", "fa0",   "fa1",  "fa2",  "fa3",  "fa4",  "fa5",
  "fa6", "fa7", "fa8",   "fa9", "fa10", "fa11", "fa12", "fa13"
};

static const char* vxpr_to_string[] = {
  "vx0",  "vx1",  "vx2",  "vx3",  "vx4",  "vx5",  "vx6",  "vx7",
  "vx8",  "vx9",  "vx10", "vx11", "vx12", "vx13", "vx14", "vx15",
  "vx16", "vx17", "vx18", "vx19", "vx20", "vx21", "vx22", "vx23",
  "vx24", "vx25", "vx26", "vx27", "vx28", "vx29", "vx30", "vx31"
};

static const char* vfpr_to_string[] = {
  "vf0",  "vf1",  "vf2",  "vf3",  "vf4",  "vf5",  "vf6",  "vf7",
  "vf8",  "vf9",  "vf10", "vf11", "vf12", "vf13", "vf14", "vf15",
  "vf16", "vf17", "vf18", "vf19", "vf20", "vf21", "vf22", "vf23",
  "vf24", "vf25", "vf26", "vf27", "vf28", "vf29", "vf30", "vf31"
};

class load_address_t : public arg_t
{
 public:
  load_address_t() {}
  virtual std::string to_string(insn_t insn) const
  {
    std::stringstream s;
    s << insn.itype.imm12 << '(' << xpr_to_string[insn.itype.rs1] << ')';
    return s.str();
  }
};

class store_address_t : public arg_t
{
 public:
  store_address_t() {}
  virtual std::string to_string(insn_t insn) const
  {
    std::stringstream s;
    int32_t imm = (int32_t)insn.btype.immlo;
    imm |= insn.btype.immhi << IMMLO_BITS;
    s << imm << '(' << xpr_to_string[insn.itype.rs1] << ')';
    return s.str();
  }
};

class amo_address_t : public arg_t
{
 public:
  amo_address_t() {}
  virtual std::string to_string(insn_t insn) const
  {
    std::stringstream s;
    s << "0(" << xpr_to_string[insn.itype.rs1] << ')';
    return s.str();
  }
};

class xrd_reg_t : public arg_t
{
 public:
  xrd_reg_t() {}
  virtual std::string to_string(insn_t insn) const
  {
    return xpr_to_string[insn.itype.rd];
  }
};

class xrs1_reg_t : public arg_t
{
 public:
  xrs1_reg_t() {}
  virtual std::string to_string(insn_t insn) const
  {
    return xpr_to_string[insn.itype.rs1];
  }
};

class xrs2_reg_t : public arg_t
{
 public:
  xrs2_reg_t() {}
  virtual std::string to_string(insn_t insn) const
  {
    return xpr_to_string[insn.rtype.rs2];
  }
};

class frd_reg_t : public arg_t
{
 public:
  frd_reg_t() {}
  virtual std::string to_string(insn_t insn) const
  {
    return fpr_to_string[insn.ftype.rd];
  }
};

class frs1_reg_t : public arg_t
{
 public:
  frs1_reg_t() {}
  virtual std::string to_string(insn_t insn) const
  {
    return fpr_to_string[insn.ftype.rs1];
  }
};

class frs2_reg_t : public arg_t
{
 public:
  frs2_reg_t() {}
  virtual std::string to_string(insn_t insn) const
  {
    return fpr_to_string[insn.ftype.rs2];
  }
};

class frs3_reg_t : public arg_t
{
 public:
  frs3_reg_t() {}
  virtual std::string to_string(insn_t insn) const
  {
    return fpr_to_string[insn.ftype.rs3];
  }
};

class vxrd_reg_t : public arg_t
{
 public:
  vxrd_reg_t() {}
  virtual std::string to_string(insn_t insn) const
  {
    return vxpr_to_string[insn.itype.rd];
  }
};

class vxrs1_reg_t : public arg_t
{
 public:
  vxrs1_reg_t() {}
  virtual std::string to_string(insn_t insn) const
  {
    return vxpr_to_string[insn.itype.rs1];
  }
};

class vfrd_reg_t : public arg_t
{
 public:
  vfrd_reg_t() {}
  virtual std::string to_string(insn_t insn) const
  {
    return vfpr_to_string[insn.itype.rd];
  }
};

class vfrs1_reg_t : public arg_t
{
 public:
  vfrs1_reg_t() {}
  virtual std::string to_string(insn_t insn) const
  {
    return vfpr_to_string[insn.itype.rs1];
  }
};

class nxregs_reg_t : public arg_t
{
 public:
  nxregs_reg_t() {}
  virtual std::string to_string(insn_t insn) const
  {
    std::stringstream s;
    s << (insn.itype.imm12 & 0x3f);
    return s.str();
  }
};

class nfregs_reg_t : public arg_t
{
 public:
  nfregs_reg_t() {}
  virtual std::string to_string(insn_t insn) const
  {
    std::stringstream s;
    s << ((insn.itype.imm12 >> 6) & 0x3f);
    return s.str();
  }
};

class pcr_reg_t : public arg_t
{
 public:
  pcr_reg_t() {}
  virtual std::string to_string(insn_t insn) const
  {
    std::stringstream s;
    s << "pcr" << insn.rtype.rs1;
    return s.str();
  }
};

class imm_t : public arg_t
{
 public:
  imm_t() {}
  virtual std::string to_string(insn_t insn) const
  {
    std::stringstream s;
    s << insn.itype.imm12;
    return s.str();
  }
};

class bigimm_t : public arg_t
{
 public:
  bigimm_t() {}
  virtual std::string to_string(insn_t insn) const
  {
    std::stringstream s;
    s << std::hex << "0x" << insn.ltype.bigimm;
    return s.str();
  }
};

class branch_target_t : public arg_t
{
 public:
  branch_target_t() {}
  virtual std::string to_string(insn_t insn) const
  {
    std::stringstream s;
    int32_t target = (int32_t)insn.btype.immlo;
    target |= insn.btype.immhi << IMMLO_BITS;
    target <<= BRANCH_ALIGN_BITS;
    char sign = target >= 0 ? '+' : '-';
    s << "pc " << sign << std::hex << " 0x" << abs(target);
    return s.str();
  }
};

class jump_target_t : public arg_t
{
 public:
  jump_target_t() {}
  virtual std::string to_string(insn_t insn) const
  {
    std::stringstream s;
    int32_t target = (int32_t)insn.jtype.target;
    target <<= JUMP_ALIGN_BITS;
    char sign = target >= 0 ? '+' : '-';
    s << "pc " << sign << std::hex << " 0x" << abs(target);
    return s.str();
  }
};

// workaround for lack of initializer_list in gcc-4.1
class disasm_insn_t
{
 public:
  disasm_insn_t(const char* name, uint32_t match, uint32_t mask)
  {
    init(name, match, mask, 0);
  }
  disasm_insn_t(const char* name, uint32_t match, uint32_t mask,
                const arg_t* a0)
  {
    init(name, match, mask, 1, a0);
  }
  disasm_insn_t(const char* name, uint32_t match, uint32_t mask,
                const arg_t* a0, const arg_t* a1)
  {
    init(name, match, mask, 2, a0, a1);
  }
  disasm_insn_t(const char* name, uint32_t match, uint32_t mask,
                const arg_t* a0, const arg_t* a1, const arg_t* a2)
  {
    init(name, match, mask, 3, a0, a1, a2);
  }
  disasm_insn_t(const char* name, uint32_t match, uint32_t mask,
                const arg_t* a0, const arg_t* a1, const arg_t* a2,
                const arg_t* a3)
  {
    init(name, match, mask, 4, a0, a1, a2, a3);
  }
  disasm_insn_t(const char* name, uint32_t match, uint32_t mask,
                const arg_t* a0, const arg_t* a1, const arg_t* a2,
                const arg_t* a3, const arg_t* a4)
  {
    init(name, match, mask, 5, a0, a1, a2, a3, a4);
  }

  bool operator == (insn_t insn) const
  {
    return (insn.bits & mask) == match;
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

  void init(const char* name, uint32_t match, uint32_t mask, int n, ...)
  {
    va_list vl;
    va_start(vl, n);
    for (int i = 0; i < n; i++)
      args.push_back(va_arg(vl, const arg_t*));
    va_end(vl);
    this->match = match;
    this->mask = mask;
    this->name = name;
  }
};

std::string disassembler::disassemble(insn_t insn)
{
  const disasm_insn_t* disasm_insn = lookup(insn);
  return disasm_insn ? disasm_insn->to_string(insn) : "unknown";
}

disassembler::disassembler()
{
  static const xrd_reg_t _xrd_reg, *xrd_reg = &_xrd_reg;
  static const xrs1_reg_t _xrs1_reg, *xrs1_reg = &_xrs1_reg;
  static const load_address_t _load_address, *load_address = &_load_address;
  static const store_address_t _store_address, *store_address = &_store_address;
  static const amo_address_t _amo_address, *amo_address = &_amo_address;
  static const xrs2_reg_t _xrs2_reg, *xrs2_reg = &_xrs2_reg;
  static const frd_reg_t _frd_reg, *frd_reg = &_frd_reg;
  static const frs1_reg_t _frs1_reg, *frs1_reg = &_frs1_reg;
  static const frs2_reg_t _frs2_reg, *frs2_reg = &_frs2_reg;
  static const frs3_reg_t _frs3_reg, *frs3_reg = &_frs3_reg;
  static const pcr_reg_t _pcr_reg, *pcr_reg = &_pcr_reg;
  static const imm_t _imm, *imm = &_imm;
  static const bigimm_t _bigimm, *bigimm = &_bigimm;
  static const branch_target_t _branch_target, *branch_target = &_branch_target;
  static const jump_target_t _jump_target, *jump_target = &_jump_target;
  static const vxrd_reg_t _vxrd_reg, *vxrd_reg = &_vxrd_reg;
  static const vxrs1_reg_t _vxrs1_reg, *vxrs1_reg = &_vxrs1_reg;
  static const vfrd_reg_t _vfrd_reg, *vfrd_reg = &_vfrd_reg;
  static const vfrs1_reg_t _vfrs1_reg, *vfrs1_reg = &_vfrs1_reg;
  static const nxregs_reg_t _nxregs_reg, *nxregs_reg = &_nxregs_reg;
  static const nfregs_reg_t _nfregs_reg, *nfregs_reg = &_nfregs_reg;

  insn_t dummy;
  dummy.bits = -1, dummy.rtype.rs1 = 0;
  uint32_t mask_rs1 = ~dummy.bits;
  dummy.bits = -1, dummy.rtype.rs2 = 0;
  uint32_t mask_rs2 = ~dummy.bits;
  dummy.bits = -1, dummy.rtype.rd = 0;
  uint32_t mask_rd = ~dummy.bits;
  dummy.bits = -1, dummy.itype.imm12 = 0;
  uint32_t mask_imm = ~dummy.bits;
  dummy.bits = 0, dummy.itype.rd = 1;
  uint32_t match_rd_ra = dummy.bits;
  dummy.bits = 0, dummy.itype.rs1 = 1;
  uint32_t match_rs1_ra = dummy.bits;

  #define DECLARE_INSN(code, match, mask) \
   const uint32_t __attribute__((unused)) match_##code = match; \
   const uint32_t __attribute__((unused)) mask_##code = mask;
  #include "opcodes.h"
  #undef DECLARE_INSN

  // explicit per-instruction disassembly
  #define DISASM_INSN(name, code, extra, ...) \
    add_insn(new disasm_insn_t(name, match_##code, mask_##code | (extra), __VA_ARGS__));
  #define DEFINE_NOARG(code) \
    add_insn(new disasm_insn_t(#code, match_##code, mask_##code));
  #define DEFINE_DTYPE(code) DISASM_INSN(#code, code, 0, xrd_reg)
  #define DEFINE_RTYPE(code) DISASM_INSN(#code, code, 0, xrd_reg, xrs1_reg, xrs2_reg)
  #define DEFINE_ITYPE(code) DISASM_INSN(#code, code, 0, xrd_reg, xrs1_reg, imm)
  #define DEFINE_I0TYPE(name, code) DISASM_INSN(name, code, mask_rs1, xrd_reg, imm)
  #define DEFINE_I1TYPE(name, code) DISASM_INSN(name, code, mask_imm, xrd_reg, xrs1_reg)
  #define DEFINE_I2TYPE(name, code) DISASM_INSN(name, code, mask_rd | mask_imm, xrs1_reg)
  #define DEFINE_LTYPE(code) DISASM_INSN(#code, code, 0, xrd_reg, bigimm)
  #define DEFINE_BTYPE(code) DISASM_INSN(#code, code, 0, xrs1_reg, xrs2_reg, branch_target)
  #define DEFINE_B0TYPE(name, code) DISASM_INSN(name, code, mask_rs1 | mask_rs2, branch_target)
  #define DEFINE_B1TYPE(name, code) DISASM_INSN(name, code, mask_rs2, xrs1_reg, branch_target)
  #define DEFINE_JTYPE(code) DISASM_INSN(#code, code, 0, jump_target)
  #define DEFINE_XLOAD(code) DISASM_INSN(#code, code, 0, xrd_reg, load_address)
  #define DEFINE_XSTORE(code) DISASM_INSN(#code, code, 0, xrs2_reg, store_address)
  #define DEFINE_XAMO(code) DISASM_INSN(#code, code, 0, xrd_reg, xrs2_reg, amo_address)
  #define DEFINE_FLOAD(code) DISASM_INSN(#code, code, 0, frd_reg, load_address)
  #define DEFINE_FSTORE(code) DISASM_INSN(#code, code, 0, frs2_reg, store_address)
  #define DEFINE_FRTYPE(code) DISASM_INSN(#code, code, 0, frd_reg, frs1_reg, frs2_reg)
  #define DEFINE_FR1TYPE(code) DISASM_INSN(#code, code, 0, frd_reg, frs1_reg)
  #define DEFINE_FR3TYPE(code) DISASM_INSN(#code, code, 0, frd_reg, frs1_reg, frs2_reg, frs3_reg)
  #define DEFINE_FXTYPE(code) DISASM_INSN(#code, code, 0, xrd_reg, frs1_reg)
  #define DEFINE_XFTYPE(code) DISASM_INSN(#code, code, 0, frd_reg, xrs1_reg)

  #define DEFINE_RS1(code) DISASM_INSN(#code, code, 0, xrs1_reg)
  #define DEFINE_RS1_RS2(code) DISASM_INSN(#code, code, 0, xrs1_reg, xrs2_reg)
  #define DEFINE_VEC_XMEM(code) DISASM_INSN(#code, code, 0, vxrd_reg, xrs1_reg)
  #define DEFINE_VEC_XMEMST(code) DISASM_INSN(#code, code, 0, vxrd_reg, xrs1_reg, xrs2_reg)
  #define DEFINE_VEC_FMEM(code) DISASM_INSN(#code, code, 0, vfrd_reg, xrs1_reg)
  #define DEFINE_VEC_FMEMST(code) DISASM_INSN(#code, code, 0, vfrd_reg, xrs1_reg, xrs2_reg)

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

  DEFINE_JTYPE(j);
  DEFINE_JTYPE(jal);

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

  DEFINE_I2TYPE("jr", jalr_j);
  add_insn(new disasm_insn_t("jalr", match_jalr_c | match_rd_ra, mask_jalr_c | mask_rd | mask_imm, xrs1_reg));
  add_insn(new disasm_insn_t("ret", match_jalr_r | match_rs1_ra, mask_jalr_r | mask_rd | mask_rs1 | mask_imm));
  DEFINE_ITYPE(jalr_c);
  DEFINE_ITYPE(jalr_r);
  DEFINE_ITYPE(jalr_j);

  add_insn(new disasm_insn_t("nop", match_addi, mask_addi | mask_rd | mask_rs1 | mask_imm));
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

  add_insn(new disasm_insn_t("mtpcr", match_mtpcr, mask_mtpcr | mask_rd, xrs2_reg, pcr_reg));
  add_insn(new disasm_insn_t("mtpcr", match_mtpcr, mask_mtpcr, xrd_reg, xrs2_reg, pcr_reg));
  add_insn(new disasm_insn_t("mfpcr", match_mfpcr, mask_mfpcr, xrd_reg, pcr_reg));
  add_insn(new disasm_insn_t("setpcr", match_setpcr, mask_setpcr, xrd_reg, pcr_reg, imm));
  add_insn(new disasm_insn_t("clearpcr", match_clearpcr, mask_clearpcr, xrd_reg, pcr_reg, imm));
  DEFINE_NOARG(eret)
  DEFINE_NOARG(cflush)

  DEFINE_RS1(vxcptsave);
  DEFINE_RS1(vxcptrestore);
  DEFINE_NOARG(vxcptkill);

  DEFINE_RS1(vxcptevac);
  DEFINE_NOARG(vxcpthold);
  DEFINE_RS1_RS2(venqcmd);
  DEFINE_RS1_RS2(venqimm1);
  DEFINE_RS1_RS2(venqimm2);
  DEFINE_RS1_RS2(venqcnt);

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
  DEFINE_XFTYPE(mxtf_s);
  DEFINE_FXTYPE(fcvt_l_s);
  DEFINE_FXTYPE(fcvt_lu_s);
  DEFINE_FXTYPE(fcvt_w_s);
  DEFINE_FXTYPE(fcvt_wu_s);
  DEFINE_FXTYPE(mftx_s);
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
  DEFINE_XFTYPE(mxtf_d);
  DEFINE_FXTYPE(fcvt_l_d);
  DEFINE_FXTYPE(fcvt_lu_d);
  DEFINE_FXTYPE(fcvt_w_d);
  DEFINE_FXTYPE(fcvt_wu_d);
  DEFINE_FXTYPE(mftx_d);
  DEFINE_FXTYPE(feq_d);
  DEFINE_FXTYPE(flt_d);
  DEFINE_FXTYPE(fle_d);

  add_insn(new disasm_insn_t("mtfsr", match_mtfsr, mask_mtfsr | mask_rd, xrs1_reg));
  add_insn(new disasm_insn_t("mtfsr", match_mtfsr, mask_mtfsr, xrd_reg, xrs1_reg));
  DEFINE_DTYPE(mffsr);

  DEFINE_VEC_XMEM(vld);
  DEFINE_VEC_XMEM(vlw);
  DEFINE_VEC_XMEM(vlwu);
  DEFINE_VEC_XMEM(vlh);
  DEFINE_VEC_XMEM(vlhu);
  DEFINE_VEC_XMEM(vlb);
  DEFINE_VEC_XMEM(vlbu);
  DEFINE_VEC_FMEM(vfld);
  DEFINE_VEC_FMEM(vflw);
  DEFINE_VEC_XMEMST(vlstd);
  DEFINE_VEC_XMEMST(vlstw);
  DEFINE_VEC_XMEMST(vlstwu);
  DEFINE_VEC_XMEMST(vlsth);
  DEFINE_VEC_XMEMST(vlsthu);
  DEFINE_VEC_XMEMST(vlstb);
  DEFINE_VEC_XMEMST(vlstbu);
  DEFINE_VEC_FMEMST(vflstd);
  DEFINE_VEC_FMEMST(vflstw);

  DEFINE_VEC_XMEM(vsd);
  DEFINE_VEC_XMEM(vsw);
  DEFINE_VEC_XMEM(vsh);
  DEFINE_VEC_XMEM(vsb);
  DEFINE_VEC_FMEM(vfsd);
  DEFINE_VEC_FMEM(vfsw);
  DEFINE_VEC_XMEMST(vsstd);
  DEFINE_VEC_XMEMST(vsstw);
  DEFINE_VEC_XMEMST(vssth);
  DEFINE_VEC_XMEMST(vsstb);
  DEFINE_VEC_FMEMST(vfsstd);
  DEFINE_VEC_FMEMST(vfsstw);

  DISASM_INSN("vmvv", vmvv, 0, vxrd_reg, vxrs1_reg);
  DISASM_INSN("vmsv", vmsv, 0, vxrd_reg, xrs1_reg);
  DISASM_INSN("vmst", vmst, 0, vxrd_reg, xrs1_reg, xrs2_reg);
  DISASM_INSN("vmts", vmts, 0, xrd_reg, vxrs1_reg, xrs2_reg);
  DISASM_INSN("vfmvv", vfmvv, 0, vfrd_reg, vfrs1_reg);
  DISASM_INSN("vfmsv", vfmsv, 0, vfrd_reg, frs1_reg);
  DISASM_INSN("vfmst", vfmst, 0, vfrd_reg, frs1_reg, frs2_reg);
  DISASM_INSN("vfmts", vfmts, 0, frd_reg, vfrs1_reg, frs2_reg);

  DEFINE_RS1_RS2(vvcfg);
  DEFINE_RS1_RS2(vtcfg);

  DISASM_INSN("vvcfgivl", vvcfgivl, 0, xrd_reg, xrs1_reg, nxregs_reg, nfregs_reg);
  DISASM_INSN("vtcfgivl", vtcfgivl, 0, xrd_reg, xrs1_reg, nxregs_reg, nfregs_reg);
  DISASM_INSN("vsetvl", vsetvl, 0, xrd_reg, xrs1_reg);
  DISASM_INSN("vf", vf, 0, xrs1_reg, imm);

  DEFINE_NOARG(fence_v_l);
  DEFINE_NOARG(fence_v_g);

  // provide a default disassembly for all instructions as a fallback
  #define DECLARE_INSN(code, match, mask) \
   add_insn(new disasm_insn_t(#code " (args unknown)", match, mask));
  #include "opcodes.h"
  #undef DECLARE_INSN
}

const disasm_insn_t* disassembler::lookup(insn_t insn)
{
  size_t idx = insn.bits % HASH_SIZE;
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
