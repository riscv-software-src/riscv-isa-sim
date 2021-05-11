// See LICENSE for license details.

#include "disasm.h"
#include <cassert>
#include <string>
#include <vector>
#include <cstdarg>
#include <sstream>
#include <stdlib.h>

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.i_imm()) + '(' + xpr_name[insn.rs1()] + ')';
  }
} load_address;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.s_imm()) + '(' + xpr_name[insn.rs1()] + ')';
  }
} store_address;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::string("(") + xpr_name[insn.rs1()] + ')';
  }
} amo_address;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return xpr_name[insn.rd()];
  }
} xrd;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return xpr_name[insn.rs1()];
  }
} xrs1;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return xpr_name[insn.rs2()];
  }
} xrs2;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return xpr_name[insn.rs3()];
  }
} xrs3;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return fpr_name[insn.rd()];
  }
} frd;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return fpr_name[insn.rs1()];
  }
} frs1;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return fpr_name[insn.rs2()];
  }
} frs2;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return fpr_name[insn.rs3()];
  }
} frs3;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    switch (insn.csr())
    {
      #define DECLARE_CSR(name, num) case num: return #name;
      #include "encoding.h"
      #undef DECLARE_CSR
      default:
      {
        char buf[16];
        snprintf(buf, sizeof buf, "unknown_%03" PRIx64, insn.csr());
        return std::string(buf);
      }
    }
  }
} csr;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.i_imm());
  }
} imm;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.shamt());
  }
} shamt;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    std::stringstream s;
    s << std::hex << "0x" << ((uint32_t)insn.u_imm() >> 12);
    return s.str();
  }
} bigimm;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string(insn.rs1());
  }
} zimm5;

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
    int32_t target = insn.uj_imm();
    char sign = target >= 0 ? '+' : '-';
    s << "pc " << sign << std::hex << " 0x" << abs(target);
    return s.str();
  }
} jump_target;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return xpr_name[insn.rvc_rs1()];
  }
} rvc_rs1;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return xpr_name[insn.rvc_rs2()];
  }
} rvc_rs2;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return fpr_name[insn.rvc_rs2()];
  }
} rvc_fp_rs2;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return xpr_name[insn.rvc_rs1s()];
  }
} rvc_rs1s;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return xpr_name[insn.rvc_rs2s()];
  }
} rvc_rs2s;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return fpr_name[insn.rvc_rs2s()];
  }
} rvc_fp_rs2s;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return xpr_name[X_SP];
  }
} rvc_sp;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.rvc_imm());
  }
} rvc_imm;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.rvc_addi4spn_imm());
  }
} rvc_addi4spn_imm;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.rvc_addi16sp_imm());
  }
} rvc_addi16sp_imm;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.rvc_lwsp_imm());
  }
} rvc_lwsp_imm;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)(insn.rvc_imm() & 0x3f));
  }
} rvc_shamt;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    std::stringstream s;
    s << std::hex << "0x" << ((uint32_t)insn.rvc_imm() << 12 >> 12);
    return s.str();
  }
} rvc_uimm;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.rvc_lwsp_imm()) + '(' + xpr_name[X_SP] + ')';
  }
} rvc_lwsp_address;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.rvc_ldsp_imm()) + '(' + xpr_name[X_SP] + ')';
  }
} rvc_ldsp_address;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.rvc_swsp_imm()) + '(' + xpr_name[X_SP] + ')';
  }
} rvc_swsp_address;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.rvc_sdsp_imm()) + '(' + xpr_name[X_SP] + ')';
  }
} rvc_sdsp_address;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.rvc_lw_imm()) + '(' + xpr_name[insn.rvc_rs1s()] + ')';
  }
} rvc_lw_address;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.rvc_ld_imm()) + '(' + xpr_name[insn.rvc_rs1s()] + ')';
  }
} rvc_ld_address;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    std::stringstream s;
    int32_t target = insn.rvc_b_imm();
    char sign = target >= 0 ? '+' : '-';
    s << "pc " << sign << ' ' << abs(target);
    return s.str();
  }
} rvc_branch_target;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    std::stringstream s;
    int32_t target = insn.rvc_j_imm();
    char sign = target >= 0 ? '+' : '-';
    s << "pc " << sign << ' ' << abs(target);
    return s.str();
  }
} rvc_jump_target;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::string("(") + xpr_name[insn.rs1()] + ')';
  }
} v_address;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return vr_name[insn.rd()];
  }
} vd;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return vr_name[insn.rs1()];
  }
} vs1;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return vr_name[insn.rs2()];
  }
} vs2;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return vr_name[insn.rd()];
  }
} vs3;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return insn.v_vm() ? "" : "v0.t";
  }
} vm;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return "v0";
  }
} v0;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.v_simm5());
  }
} v_simm5;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    std::stringstream s;
    int sew = insn.v_sew();
    int lmul = insn.v_lmul();
    auto vta = insn.v_vta() == 1 ? "ta" : "tu";
    auto vma = insn.v_vma() == 1 ? "ma" : "mu";
    s << "e" << sew;
    if(insn.v_frac_lmul()) {
      std::string lmul_str = "";
      switch(lmul){
        case 3:
          lmul_str = "f2";
          break;
        case 2:
          lmul_str = "f4";
          break;
        case 1:
          lmul_str = "f8";
          break;
        default:
          assert(true && "unsupport fractional LMUL");
      }
      s << ", m" << lmul_str;
    } else {
      s << ", m" << (1 << lmul);
    }
    s << ", " << vta << ", " << vma;
    return s.str();
  }
} v_vtype;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return "x0";
  }
} x0;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    std::stringstream s;
    auto iorw = insn.iorw();
    bool has_pre = false;
    static const char type[] = "wroi";
    for (int i = 7; i >= 4; --i) {
      if (iorw & (1ul << i)) {
        s << type[i - 4];
        has_pre = true;
      }
    }

    s << (has_pre ? "," : "");
    for (int i = 3; i >= 0; --i) {
      if (iorw & (1ul << i)) {
        s << type[i];
      }
    }

    return s.str();
  }
} iorw;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.p_imm2());
  }
} p_imm2;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.p_imm3());
  }
} p_imm3;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.p_imm4());
  }
} p_imm4;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.p_imm5());
  }
} p_imm5;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.p_imm6());
  }
} p_imm6;

typedef struct {
  reg_t match;
  reg_t mask;
  const char *fmt;
  std::vector<const arg_t*>& arg;
} custom_fmt_t;

std::string disassembler_t::disassemble(insn_t insn) const
{
  const disasm_insn_t* disasm_insn = lookup(insn);
  return disasm_insn ? disasm_insn->to_string(insn) : "unknown";
}

disassembler_t::disassembler_t(int xlen)
{
  const uint32_t mask_rd = 0x1fUL << 7;
  const uint32_t match_rd_ra = 1UL << 7;
  const uint32_t mask_rs1 = 0x1fUL << 15;
  const uint32_t match_rs1_ra = 1UL << 15;
  const uint32_t mask_rs2 = 0x1fUL << 20;
  const uint32_t mask_imm = 0xfffUL << 20;
  const uint32_t imm_shift = 20;
  const uint32_t mask_rvc_rs2 = 0x1fUL << 2;
  const uint32_t mask_rvc_imm = mask_rvc_rs2 | 0x1000UL;
  const uint32_t mask_nf = 0x7Ul << 29;
  const uint32_t mask_wd = 0x1Ul << 26;
  const uint32_t mask_vm = 0x1Ul << 25;
  const uint32_t mask_vldst = 0x7Ul << 12 | 0x1UL << 28;
  const uint32_t mask_amoop = 0x1fUl << 27;
  const uint32_t mask_width = 0x7Ul << 12;

  #define DECLARE_INSN(code, match, mask) \
   const uint32_t match_##code = match; \
   const uint32_t mask_##code = mask;
  #define DECLARE_RV32_ONLY(code) {}
  #define DECLARE_RV64_ONLY(code) {}
  #include "encoding.h"
  #undef DECLARE_RV64_INSN
  #undef DECLARE_RV32_INSN
  #undef DECLARE_INSN

  // explicit per-instruction disassembly
  #define DISASM_INSN(name, code, extra, ...) \
    add_insn(new disasm_insn_t(name, match_##code, mask_##code | (extra), __VA_ARGS__));
  #define DEFINE_NOARG(code) \
    add_insn(new disasm_insn_t(#code, match_##code, mask_##code, {}));
  #define DEFINE_RTYPE(code) DISASM_INSN(#code, code, 0, {&xrd, &xrs1, &xrs2})
  #define DEFINE_R1TYPE(code) DISASM_INSN(#code, code, 0, {&xrd, &xrs1})
  #define DEFINE_ITYPE(code) DISASM_INSN(#code, code, 0, {&xrd, &xrs1, &imm})
  #define DEFINE_ITYPE_SHIFT(code) DISASM_INSN(#code, code, 0, {&xrd, &xrs1, &shamt})
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
  #define DEFINE_XAMO_LR(code) DISASM_INSN(#code, code, 0, {&xrd, &amo_address})
  #define DEFINE_FLOAD(code) DISASM_INSN(#code, code, 0, {&frd, &load_address})
  #define DEFINE_FSTORE(code) DISASM_INSN(#code, code, 0, {&frs2, &store_address})
  #define DEFINE_FRTYPE(code) DISASM_INSN(#code, code, 0, {&frd, &frs1, &frs2})
  #define DEFINE_FR1TYPE(code) DISASM_INSN(#code, code, 0, {&frd, &frs1})
  #define DEFINE_FR3TYPE(code) DISASM_INSN(#code, code, 0, {&frd, &frs1, &frs2, &frs3})
  #define DEFINE_FXTYPE(code) DISASM_INSN(#code, code, 0, {&xrd, &frs1})
  #define DEFINE_FX2TYPE(code) DISASM_INSN(#code, code, 0, {&xrd, &frs1, &frs2})
  #define DEFINE_XFTYPE(code) DISASM_INSN(#code, code, 0, {&frd, &xrs1})
  #define DEFINE_SFENCE_TYPE(code) DISASM_INSN(#code, code, 0, {&xrs1, &xrs2})

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
  DEFINE_XAMO(amoxor_w)
  DEFINE_XAMO(amomin_w)
  DEFINE_XAMO(amomax_w)
  DEFINE_XAMO(amominu_w)
  DEFINE_XAMO(amomaxu_w)
  DEFINE_XAMO(amoadd_d)
  DEFINE_XAMO(amoswap_d)
  DEFINE_XAMO(amoand_d)
  DEFINE_XAMO(amoor_d)
  DEFINE_XAMO(amoxor_d)
  DEFINE_XAMO(amomin_d)
  DEFINE_XAMO(amomax_d)
  DEFINE_XAMO(amominu_d)
  DEFINE_XAMO(amomaxu_d)

  DEFINE_XAMO_LR(lr_w)
  DEFINE_XAMO(sc_w)
  DEFINE_XAMO_LR(lr_d)
  DEFINE_XAMO(sc_d)

  DEFINE_FLOAD(flw)
  DEFINE_FLOAD(fld)
  DEFINE_FLOAD(flh)
  DEFINE_FLOAD(flq)

  DEFINE_FSTORE(fsw)
  DEFINE_FSTORE(fsd)
  DEFINE_FSTORE(fsh)
  DEFINE_FSTORE(fsq)

  add_insn(new disasm_insn_t("j", match_jal, mask_jal | mask_rd, {&jump_target}));
  add_insn(new disasm_insn_t("jal", match_jal | match_rd_ra, mask_jal | mask_rd, {&jump_target}));
  add_insn(new disasm_insn_t("jal", match_jal, mask_jal, {&xrd, &jump_target}));

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

  add_insn(new disasm_insn_t("ret", match_jalr | match_rs1_ra, mask_jalr | mask_rd | mask_rs1 | mask_imm, {}));
  DEFINE_I2TYPE("jr", jalr);
  add_insn(new disasm_insn_t("jalr", match_jalr | match_rd_ra, mask_jalr | mask_rd | mask_imm, {&xrs1}));
  DEFINE_ITYPE(jalr);

  add_insn(new disasm_insn_t("nop", match_addi, mask_addi | mask_rd | mask_rs1 | mask_imm, {}));
  DEFINE_I0TYPE("li", addi);
  DEFINE_I1TYPE("mv", addi);
  DEFINE_ITYPE(addi);
  DEFINE_ITYPE(slti);
  add_insn(new disasm_insn_t("seqz", match_sltiu | (1 << imm_shift), mask_sltiu | mask_imm, {&xrd, &xrs1}));
  DEFINE_ITYPE(sltiu);
  add_insn(new disasm_insn_t("not", match_xori | mask_imm, mask_xori | mask_imm, {&xrd, &xrs1}));
  DEFINE_ITYPE(xori);

  DEFINE_ITYPE_SHIFT(slli);
  DEFINE_ITYPE_SHIFT(srli);
  DEFINE_ITYPE_SHIFT(srai);

  DEFINE_ITYPE(ori);
  DEFINE_ITYPE(andi);
  DEFINE_I1TYPE("sext.w", addiw);
  DEFINE_ITYPE(addiw);

  DEFINE_ITYPE_SHIFT(slliw);
  DEFINE_ITYPE_SHIFT(srliw);
  DEFINE_ITYPE_SHIFT(sraiw);

  DEFINE_RTYPE(add);
  DEFINE_RTYPE(sub);
  DEFINE_RTYPE(sll);
  DEFINE_RTYPE(slt);
  add_insn(new disasm_insn_t("snez", match_sltu, mask_sltu | mask_rs1, {&xrd, &xrs2}));
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

  DEFINE_ITYPE_SHIFT(slli_uw);
  add_insn(new disasm_insn_t("zext.w", match_add_uw, mask_add_uw | mask_rs2, {&xrd, &xrs1}));
  DEFINE_RTYPE(add_uw);
  DEFINE_RTYPE(sh1add);
  DEFINE_RTYPE(sh2add);
  DEFINE_RTYPE(sh3add);
  DEFINE_RTYPE(sh1add_uw);
  DEFINE_RTYPE(sh2add_uw);
  DEFINE_RTYPE(sh3add_uw);
  DEFINE_RTYPE(ror);
  DEFINE_RTYPE(rorw);
  DEFINE_RTYPE(rol);
  DEFINE_RTYPE(rolw);
  DEFINE_ITYPE_SHIFT(rori);
  DEFINE_ITYPE_SHIFT(roriw);
  DEFINE_R1TYPE(ctz);
  DEFINE_R1TYPE(ctzw);
  DEFINE_R1TYPE(clz);
  DEFINE_R1TYPE(clzw);
  DEFINE_R1TYPE(cpop);
  DEFINE_R1TYPE(cpopw);
  DEFINE_RTYPE(min);
  DEFINE_RTYPE(minu);
  DEFINE_RTYPE(max);
  DEFINE_RTYPE(maxu);
  DEFINE_RTYPE(andn);
  DEFINE_RTYPE(orn);
  DEFINE_RTYPE(xnor);
  DEFINE_R1TYPE(sext_b);
  DEFINE_R1TYPE(sext_h);
  add_insn(new disasm_insn_t("zext.h", (xlen == 32 ? match_pack : match_packw), mask_pack | mask_rs2, {&xrd, &xrs1}));
  DEFINE_RTYPE(pack);
  DEFINE_RTYPE(packw);
  DEFINE_RTYPE(grev);
  add_insn(new disasm_insn_t("rev8", match_grevi | ((xlen == 32 ? 0x18 : 0x38) << imm_shift), mask_grevi | mask_imm, {&xrd, &xrs1}));
  DEFINE_ITYPE_SHIFT(grevi);
  DEFINE_RTYPE(gorc);
  add_insn(new disasm_insn_t("orc.b", match_gorci | (0x7 << imm_shift), mask_grevi | mask_imm, {&xrd, &xrs1}));
  DEFINE_ITYPE_SHIFT(gorci);
  DEFINE_RTYPE(xperm_n);
  DEFINE_RTYPE(xperm_b);
  DEFINE_RTYPE(xperm_h);
  DEFINE_RTYPE(xperm_w);

  DEFINE_NOARG(ecall);
  DEFINE_NOARG(ebreak);
  DEFINE_NOARG(uret);
  DEFINE_NOARG(sret);
  DEFINE_NOARG(mret);
  DEFINE_NOARG(dret);
  DEFINE_NOARG(wfi);
  add_insn(new disasm_insn_t("fence", match_fence, mask_fence, {&iorw}));
  DEFINE_NOARG(fence_i);
  DEFINE_SFENCE_TYPE(sfence_vma);

  add_insn(new disasm_insn_t("csrr", match_csrrs, mask_csrrs | mask_rs1, {&xrd, &csr}));
  add_insn(new disasm_insn_t("csrw", match_csrrw, mask_csrrw | mask_rd, {&csr, &xrs1}));
  add_insn(new disasm_insn_t("csrs", match_csrrs, mask_csrrs | mask_rd, {&csr, &xrs1}));
  add_insn(new disasm_insn_t("csrc", match_csrrc, mask_csrrc | mask_rd, {&csr, &xrs1}));
  add_insn(new disasm_insn_t("csrwi", match_csrrwi, mask_csrrwi | mask_rd, {&csr, &zimm5}));
  add_insn(new disasm_insn_t("csrsi", match_csrrsi, mask_csrrsi | mask_rd, {&csr, &zimm5}));
  add_insn(new disasm_insn_t("csrci", match_csrrci, mask_csrrci | mask_rd, {&csr, &zimm5}));
  add_insn(new disasm_insn_t("csrrw", match_csrrw, mask_csrrw, {&xrd, &csr, &xrs1}));
  add_insn(new disasm_insn_t("csrrs", match_csrrs, mask_csrrs, {&xrd, &csr, &xrs1}));
  add_insn(new disasm_insn_t("csrrc", match_csrrc, mask_csrrc, {&xrd, &csr, &xrs1}));
  add_insn(new disasm_insn_t("csrrwi", match_csrrwi, mask_csrrwi, {&xrd, &csr, &zimm5}));
  add_insn(new disasm_insn_t("csrrsi", match_csrrsi, mask_csrrsi, {&xrd, &csr, &zimm5}));
  add_insn(new disasm_insn_t("csrrci", match_csrrci, mask_csrrci, {&xrd, &csr, &zimm5}));

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
  DEFINE_FR1TYPE(fcvt_s_q);
  DEFINE_XFTYPE(fcvt_s_l);
  DEFINE_XFTYPE(fcvt_s_lu);
  DEFINE_XFTYPE(fcvt_s_w);
  DEFINE_XFTYPE(fcvt_s_wu);
  DEFINE_XFTYPE(fcvt_s_wu);
  DEFINE_XFTYPE(fmv_w_x);
  DEFINE_FXTYPE(fcvt_l_s);
  DEFINE_FXTYPE(fcvt_lu_s);
  DEFINE_FXTYPE(fcvt_w_s);
  DEFINE_FXTYPE(fcvt_wu_s);
  DEFINE_FXTYPE(fclass_s);
  DEFINE_FXTYPE(fmv_x_w);
  DEFINE_FX2TYPE(feq_s);
  DEFINE_FX2TYPE(flt_s);
  DEFINE_FX2TYPE(fle_s);

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
  DEFINE_FR1TYPE(fcvt_d_q);
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
  DEFINE_FXTYPE(fclass_d);
  DEFINE_FXTYPE(fmv_x_d);
  DEFINE_FX2TYPE(feq_d);
  DEFINE_FX2TYPE(flt_d);
  DEFINE_FX2TYPE(fle_d);

  DEFINE_FRTYPE(fadd_h);
  DEFINE_FRTYPE(fsub_h);
  DEFINE_FRTYPE(fmul_h);
  DEFINE_FRTYPE(fdiv_h);
  DEFINE_FR1TYPE(fsqrt_h);
  DEFINE_FRTYPE(fmin_h);
  DEFINE_FRTYPE(fmax_h);
  DEFINE_FR3TYPE(fmadd_h);
  DEFINE_FR3TYPE(fmsub_h);
  DEFINE_FR3TYPE(fnmadd_h);
  DEFINE_FR3TYPE(fnmsub_h);
  DEFINE_FRTYPE(fsgnj_h);
  DEFINE_FRTYPE(fsgnjn_h);
  DEFINE_FRTYPE(fsgnjx_h);
  DEFINE_FR1TYPE(fcvt_h_s);
  DEFINE_FR1TYPE(fcvt_h_d);
  DEFINE_FR1TYPE(fcvt_h_q);
  DEFINE_FR1TYPE(fcvt_s_h);
  DEFINE_FR1TYPE(fcvt_d_h);
  DEFINE_FR1TYPE(fcvt_q_h);
  DEFINE_XFTYPE(fcvt_h_l);
  DEFINE_XFTYPE(fcvt_h_lu);
  DEFINE_XFTYPE(fcvt_h_w);
  DEFINE_XFTYPE(fcvt_h_wu);
  DEFINE_XFTYPE(fcvt_h_wu);
  DEFINE_XFTYPE(fmv_h_x);
  DEFINE_FXTYPE(fcvt_l_h);
  DEFINE_FXTYPE(fcvt_lu_h);
  DEFINE_FXTYPE(fcvt_w_h);
  DEFINE_FXTYPE(fcvt_wu_h);
  DEFINE_FXTYPE(fclass_h);
  DEFINE_FXTYPE(fmv_x_h);
  DEFINE_FX2TYPE(feq_h);
  DEFINE_FX2TYPE(flt_h);
  DEFINE_FX2TYPE(fle_h);

  DEFINE_FRTYPE(fadd_q);
  DEFINE_FRTYPE(fsub_q);
  DEFINE_FRTYPE(fmul_q);
  DEFINE_FRTYPE(fdiv_q);
  DEFINE_FR1TYPE(fsqrt_q);
  DEFINE_FRTYPE(fmin_q);
  DEFINE_FRTYPE(fmax_q);
  DEFINE_FR3TYPE(fmadd_q);
  DEFINE_FR3TYPE(fmsub_q);
  DEFINE_FR3TYPE(fnmadd_q);
  DEFINE_FR3TYPE(fnmsub_q);
  DEFINE_FRTYPE(fsgnj_q);
  DEFINE_FRTYPE(fsgnjn_q);
  DEFINE_FRTYPE(fsgnjx_q);
  DEFINE_FR1TYPE(fcvt_q_s);
  DEFINE_FR1TYPE(fcvt_q_d);
  DEFINE_XFTYPE(fcvt_q_l);
  DEFINE_XFTYPE(fcvt_q_lu);
  DEFINE_XFTYPE(fcvt_q_w);
  DEFINE_XFTYPE(fcvt_q_wu);
  DEFINE_XFTYPE(fcvt_q_wu);
  DEFINE_FXTYPE(fcvt_l_q);
  DEFINE_FXTYPE(fcvt_lu_q);
  DEFINE_FXTYPE(fcvt_w_q);
  DEFINE_FXTYPE(fcvt_wu_q);
  DEFINE_FXTYPE(fclass_q);
  DEFINE_FX2TYPE(feq_q);
  DEFINE_FX2TYPE(flt_q);
  DEFINE_FX2TYPE(fle_q);

  DISASM_INSN("c.ebreak", c_add, mask_rd | mask_rvc_rs2, {});
  add_insn(new disasm_insn_t("ret", match_c_jr | match_rd_ra, mask_c_jr | mask_rd | mask_rvc_imm, {}));
  DISASM_INSN("c.jr", c_jr, mask_rvc_imm, {&rvc_rs1});
  DISASM_INSN("c.jalr", c_jalr, mask_rvc_imm, {&rvc_rs1});
  DISASM_INSN("c.nop", c_addi, mask_rd | mask_rvc_imm, {});
  DISASM_INSN("c.addi16sp", c_addi16sp, mask_rd, {&rvc_sp, &rvc_addi16sp_imm});
  DISASM_INSN("c.addi4spn", c_addi4spn, 0, {&rvc_rs2s, &rvc_sp, &rvc_addi4spn_imm});
  DISASM_INSN("c.li", c_li, 0, {&xrd, &rvc_imm});
  DISASM_INSN("c.lui", c_lui, 0, {&xrd, &rvc_uimm});
  DISASM_INSN("c.addi", c_addi, 0, {&xrd, &rvc_imm});
  DISASM_INSN("c.slli", c_slli, 0, {&rvc_rs1, &rvc_shamt});
  DISASM_INSN("c.srli", c_srli, 0, {&rvc_rs1s, &rvc_shamt});
  DISASM_INSN("c.srai", c_srai, 0, {&rvc_rs1s, &rvc_shamt});
  DISASM_INSN("c.andi", c_andi, 0, {&rvc_rs1s, &rvc_imm});
  DISASM_INSN("c.mv", c_mv, 0, {&xrd, &rvc_rs2});
  DISASM_INSN("c.add", c_add, 0, {&xrd, &rvc_rs2});
  DISASM_INSN("c.addw", c_addw, 0, {&rvc_rs1s, &rvc_rs2s});
  DISASM_INSN("c.sub", c_sub, 0, {&rvc_rs1s, &rvc_rs2s});
  DISASM_INSN("c.subw", c_subw, 0, {&rvc_rs1s, &rvc_rs2s});
  DISASM_INSN("c.and", c_and, 0, {&rvc_rs1s, &rvc_rs2s});
  DISASM_INSN("c.or", c_or, 0, {&rvc_rs1s, &rvc_rs2s});
  DISASM_INSN("c.xor", c_xor, 0, {&rvc_rs1s, &rvc_rs2s});
  DISASM_INSN("c.lwsp", c_lwsp, 0, {&xrd, &rvc_lwsp_address});
  DISASM_INSN("c.fld", c_fld, 0, {&rvc_fp_rs2s, &rvc_ld_address});
  DISASM_INSN("c.swsp", c_swsp, 0, {&rvc_rs2, &rvc_swsp_address});
  DISASM_INSN("c.lw", c_lw, 0, {&rvc_rs2s, &rvc_lw_address});
  DISASM_INSN("c.sw", c_sw, 0, {&rvc_rs2s, &rvc_lw_address});
  DISASM_INSN("c.beqz", c_beqz, 0, {&rvc_rs1s, &rvc_branch_target});
  DISASM_INSN("c.bnez", c_bnez, 0, {&rvc_rs1s, &rvc_branch_target});
  DISASM_INSN("c.j", c_j, 0, {&rvc_jump_target});
  DISASM_INSN("c.fldsp", c_fldsp, 0, {&frd, &rvc_ldsp_address});
  DISASM_INSN("c.fsd", c_fsd, 0, {&rvc_fp_rs2s, &rvc_ld_address});
  DISASM_INSN("c.fsdsp", c_fsdsp, 0, {&rvc_fp_rs2, &rvc_sdsp_address});

  DISASM_INSN("vsetivli", vsetivli, 0, {&xrd, &zimm5, &v_vtype});
  DISASM_INSN("vsetvli", vsetvli, 0, {&xrd, &xrs1, &v_vtype});
  DISASM_INSN("vsetvl", vsetvl, 0, {&xrd, &xrs1, &xrs2});

  #define DISASM_VMEM_INSN(name, fmt, ff) \
    add_insn(new disasm_insn_t(#name "8"    #ff ".v",  match_##name##8##ff##_v,     mask_##name##8##ff##_v    | mask_nf, fmt)); \
    add_insn(new disasm_insn_t(#name "16"   #ff ".v",  match_##name##16##ff##_v,    mask_##name##16##ff##_v   | mask_nf, fmt)); \
    add_insn(new disasm_insn_t(#name "32"   #ff ".v",  match_##name##32##ff##_v,    mask_##name##32##ff##_v   | mask_nf, fmt)); \
    add_insn(new disasm_insn_t(#name "64"   #ff ".v",  match_##name##64##ff##_v,    mask_##name##64##ff##_v   | mask_nf, fmt)); \

  std::vector<const arg_t *> v_ld_unit = {&vd, &v_address, &opt, &vm};
  std::vector<const arg_t *> v_st_unit = {&vs3, &v_address, &opt, &vm};
  std::vector<const arg_t *> v_ld_stride = {&vd, &v_address, &xrs2, &opt, &vm};
  std::vector<const arg_t *> v_st_stride = {&vs3, &v_address, &xrs2, &opt, &vm};
  std::vector<const arg_t *> v_ld_index = {&vd, &v_address, &vs2, &opt, &vm};
  std::vector<const arg_t *> v_st_index = {&vs3, &v_address, &vs2, &opt, &vm};

  add_insn(new disasm_insn_t("vle1.v",  match_vle1_v,     mask_vle1_v, v_ld_unit));
  add_insn(new disasm_insn_t("vse1.v",  match_vse1_v,     mask_vse1_v, v_st_unit));

  DISASM_VMEM_INSN(vle,    v_ld_unit,   );
  DISASM_VMEM_INSN(vluxei, v_ld_index,  );
  DISASM_VMEM_INSN(vlse,   v_ld_stride, );
  DISASM_VMEM_INSN(vloxei, v_ld_index,  );
  DISASM_VMEM_INSN(vle,    v_ld_unit, ff);
  DISASM_VMEM_INSN(vse,    v_st_unit,   );
  DISASM_VMEM_INSN(vsoxei, v_st_index,  );
  DISASM_VMEM_INSN(vsse,   v_st_stride, );
  DISASM_VMEM_INSN(vsuxei, v_st_index,  );

  #undef DISASM_VMEM_INSN

  // handle vector segment load/store
  for (size_t elt = 0; elt <= 7; ++elt) {
    const custom_fmt_t template_insn[] = {
      {match_vle8_v,   mask_vle8_v,   "vlseg%de%d.v",   v_ld_unit},
      {match_vse8_v,   mask_vse8_v,   "vsseg%de%d.v",   v_st_unit},

      {match_vluxei8_v, mask_vluxei8_v, "vluxseg%dei%d.v", v_ld_index},
      {match_vsuxei8_v, mask_vsuxei8_v, "vsuxseg%dei%d.v", v_st_index},

      {match_vlse8_v,  mask_vlse8_v,  "vlsseg%de%d.v",  v_ld_stride},
      {match_vsse8_v,  mask_vsse8_v,  "vssseg%de%d.v",  v_st_stride},

      {match_vloxei8_v, mask_vloxei8_v, "vloxseg%dei%d.v", v_ld_index},
      {match_vsoxei8_v, mask_vsoxei8_v, "vsoxseg%dei%d.v", v_st_index},

      {match_vle8ff_v, mask_vle8ff_v, "vlseg%de%dff.v", v_ld_unit}
    };

    reg_t elt_map[] = {0x00000000, 0x00005000, 0x00006000, 0x00007000,
                       0x10000000, 0x10005000, 0x10006000, 0x10007000};

    for (size_t nf = 1; nf <= 7; ++nf) {
      for (auto item : template_insn) {
        const reg_t match_nf = nf << 29;
        char buf[128];
        sprintf(buf, item.fmt, nf + 1, 8 << elt);
        add_insn(new disasm_insn_t(
          buf,
          ((item.match | match_nf) & ~mask_vldst) | elt_map[elt],
          item.mask | mask_nf,
          item.arg
          ));
      }
    }

    //handle whole register load
    if (elt >= 4)
        continue;

    const custom_fmt_t template_insn2[] = {
      {match_vl1re8_v,   mask_vl1re8_v,   "vl%dre%d.v",   v_ld_unit},
    };

    for (reg_t i = 0, nf = 7; i < 4; i++, nf >>= 1) {
      for (auto item : template_insn2) {
        const reg_t match_nf = nf << 29;
        char buf[128];
        sprintf(buf, item.fmt, nf + 1, 8 << elt);
        add_insn(new disasm_insn_t(
          buf,
          item.match | match_nf | elt_map[elt],
          item.mask | mask_nf,
          item.arg
        ));
      }
    }
  }

  #define DISASM_ST_WHOLE_INSN(name, nf) \
    add_insn(new disasm_insn_t(#name, match_vs1r_v | (nf << 29), \
                                      mask_vs1r_v | mask_nf, \
                                      {&vs3, &v_address}));
  DISASM_ST_WHOLE_INSN(vs1r.v, 0);
  DISASM_ST_WHOLE_INSN(vs2r.v, 1);
  DISASM_ST_WHOLE_INSN(vs4r.v, 3);
  DISASM_ST_WHOLE_INSN(vs8r.v, 7);

  #undef DISASM_ST_WHOLE_INSN

  #define DISASM_OPIV_VXI_INSN(name, sign, suf) \
    add_insn(new disasm_insn_t(#name "." #suf "v", \
                match_##name##_##suf##v, mask_##name##_##suf##v, \
                {&vd, &vs2, &vs1, &opt, &vm})); \
    add_insn(new disasm_insn_t(#name "." #suf "x", \
                match_##name##_##suf##x, mask_##name##_##suf##x, \
                {&vd, &vs2, &xrs1, &opt, &vm})); \
    if (sign) \
    add_insn(new disasm_insn_t(#name "." #suf "i", \
                match_##name##_##suf##i, mask_##name##_##suf##i, \
                {&vd, &vs2, &v_simm5, &opt, &vm})); \
    else \
    add_insn(new disasm_insn_t(#name "." #suf "i", \
                match_##name##_##suf##i, mask_##name##_##suf##i, \
                {&vd, &vs2, &zimm5, &opt, &vm}));

  #define DISASM_OPIV_VX__INSN(name, sign) \
    add_insn(new disasm_insn_t(#name ".vv", match_##name##_vv, mask_##name##_vv, \
                {&vd, &vs2, &vs1, &opt, &vm})); \
    add_insn(new disasm_insn_t(#name ".vx", match_##name##_vx, mask_##name##_vx, \
                {&vd, &vs2, &xrs1, &opt, &vm})); \

  #define DISASM_OPIV__XI_INSN(name, sign) \
    add_insn(new disasm_insn_t(#name ".vx", match_##name##_vx, mask_##name##_vx, \
                {&vd, &vs2, &xrs1, &opt, &vm})); \
    if (sign) \
    add_insn(new disasm_insn_t(#name ".vi", match_##name##_vi, mask_##name##_vi, \
                {&vd, &vs2, &v_simm5, &opt, &vm})); \
    else \
    add_insn(new disasm_insn_t(#name ".vi", match_##name##_vi, mask_##name##_vi, \
                {&vd, &vs2, &zimm5, &opt, &vm}));

  #define DISASM_OPIV_V___INSN(name, sign) \
    add_insn(new disasm_insn_t(#name ".vv", match_##name##_vv, mask_##name##_vv, \
                {&vd, &vs2, &vs1, &opt, &vm}));

  #define DISASM_OPIV_S___INSN(name, sign) \
    add_insn(new disasm_insn_t(#name ".vs", match_##name##_vs, mask_##name##_vs, \
                {&vd, &vs2, &vs1, &opt, &vm}));

  #define DISASM_OPIV_W___INSN(name, sign) \
    add_insn(new disasm_insn_t(#name ".wv", match_##name##_wv, mask_##name##_wv, \
                {&vd, &vs2, &vs1, &opt, &vm})); \
    add_insn(new disasm_insn_t(#name ".wx", match_##name##_wx, mask_##name##_wx, \
                {&vd, &vs2, &xrs1, &opt, &vm}));

  #define DISASM_OPIV_M___INSN(name, sign) \
    add_insn(new disasm_insn_t(#name ".mm", match_##name##_mm, mask_##name##_mm, \
                {&vd, &vs2, &vs1}));

  #define DISASM_OPIV__X__INSN(name, sign) \
    add_insn(new disasm_insn_t(#name ".vx", match_##name##_vx, mask_##name##_vx, \
                {&vd, &vs2, &xrs1, &opt, &vm}));

  #define DISASM_OPIV_VXIM_INSN(name, sign, has_vm) \
    add_insn(new disasm_insn_t(#name ".vvm", match_##name##_vvm, \
                mask_##name##_vvm | mask_vm, \
                {&vd, &vs2, &vs1, &v0})); \
    add_insn(new disasm_insn_t(#name ".vxm", match_##name##_vxm, \
                mask_##name##_vxm | mask_vm, \
                {&vd, &vs2, &xrs1, &v0})); \
    add_insn(new disasm_insn_t(#name ".vim", match_##name##_vim, \
                mask_##name##_vim | mask_vm, \
                {&vd, &vs2, &v_simm5, &v0})); \
    if (has_vm) { \
        add_insn(new disasm_insn_t(#name ".vv", \
                    match_##name##_vvm | mask_vm, \
                    mask_##name##_vvm | mask_vm, \
                    {&vd, &vs2, &vs1})); \
        add_insn(new disasm_insn_t(#name ".vx", \
                    match_##name##_vxm | mask_vm, \
                    mask_##name##_vxm | mask_vm, \
                    {&vd, &vs2, &xrs1})); \
        add_insn(new disasm_insn_t(#name ".vi", \
                    match_##name##_vim | mask_vm, \
                    mask_##name##_vim | mask_vm, \
                    {&vd, &vs2, &v_simm5})); \
    }

  #define DISASM_OPIV_VX_M_INSN(name, sign, has_vm) \
    add_insn(new disasm_insn_t(#name ".vvm", match_##name##_vvm, \
                mask_##name##_vvm | mask_vm, \
                {&vd, &vs2, &vs1, &v0})); \
    add_insn(new disasm_insn_t(#name ".vxm", match_##name##_vxm, \
                mask_##name##_vxm | mask_vm, \
                {&vd, &vs2, &xrs1, &v0})); \
    if (has_vm) { \
        add_insn(new disasm_insn_t(#name ".vv", \
                    match_##name##_vvm | mask_vm, \
                    mask_##name##_vvm | mask_vm, \
                    {&vd, &vs2, &vs1})); \
        add_insn(new disasm_insn_t(#name ".vx", \
                    match_##name##_vxm | mask_vm, \
                    mask_##name##_vxm | mask_vm, \
                    {&vd, &vs2, &xrs1})); \
    } \

  //OPFVV/OPFVF
  //0b00_0000
  DISASM_OPIV_VXI_INSN(vadd,         1, v);
  DISASM_OPIV_VX__INSN(vsub,         1);
  DISASM_OPIV__XI_INSN(vrsub,        1);
  DISASM_OPIV_VX__INSN(vminu,        0);
  DISASM_OPIV_VX__INSN(vmin,         1);
  DISASM_OPIV_VX__INSN(vmaxu,        1);
  DISASM_OPIV_VX__INSN(vmax,         0);
  DISASM_OPIV_VXI_INSN(vand,         1, v);
  DISASM_OPIV_VXI_INSN(vor,          1, v);
  DISASM_OPIV_VXI_INSN(vxor,         1, v);
  DISASM_OPIV_VXI_INSN(vrgather,     0, v);
  DISASM_OPIV_V___INSN(vrgatherei16, 0);
  DISASM_OPIV__XI_INSN(vslideup,     0);
  DISASM_OPIV__XI_INSN(vslidedown,   0);

  //0b01_0000
  DISASM_OPIV_VXIM_INSN(vadc,    1, 0);
  DISASM_OPIV_VXIM_INSN(vmadc,   1, 1);
  DISASM_OPIV_VX_M_INSN(vsbc,    1, 0);
  DISASM_OPIV_VX_M_INSN(vmsbc,   1, 1);
  DISASM_OPIV_VXIM_INSN(vmerge,  1, 0);
  DISASM_INSN("vmv.v.i", vmv_v_i, 0, {&vd, &v_simm5});
  DISASM_INSN("vmv.v.v", vmv_v_v, 0, {&vd, &vs1});
  DISASM_INSN("vmv.v.x", vmv_v_x, 0, {&vd, &xrs1});
  DISASM_OPIV_VXI_INSN(vmseq,     1, v);
  DISASM_OPIV_VXI_INSN(vmsne,     1, v);
  DISASM_OPIV_VX__INSN(vmsltu,    0);
  DISASM_OPIV_VX__INSN(vmslt,     1);
  DISASM_OPIV_VXI_INSN(vmsleu,    0, v);
  DISASM_OPIV_VXI_INSN(vmsle,     1, v);
  DISASM_OPIV__XI_INSN(vmsgtu,    0);
  DISASM_OPIV__XI_INSN(vmsgt,     1);

  //0b10_0000
  DISASM_OPIV_VXI_INSN(vsaddu,    0, v);
  DISASM_OPIV_VXI_INSN(vsadd,     1, v);
  DISASM_OPIV_VX__INSN(vssubu,    0);
  DISASM_OPIV_VX__INSN(vssub,     1);
  DISASM_OPIV_VXI_INSN(vsll,      1, v);
  DISASM_INSN("vmv1r.v", vmv1r_v, 0, {&vd, &vs2});
  DISASM_INSN("vmv2r.v", vmv2r_v, 0, {&vd, &vs2});
  DISASM_INSN("vmv4r.v", vmv4r_v, 0, {&vd, &vs2});
  DISASM_INSN("vmv8r.v", vmv8r_v, 0, {&vd, &vs2});
  DISASM_OPIV_VX__INSN(vsmul,     1);
  DISASM_OPIV_VXI_INSN(vsrl,      0, v);
  DISASM_OPIV_VXI_INSN(vsra,      0, v);
  DISASM_OPIV_VXI_INSN(vssrl,     0, v);
  DISASM_OPIV_VXI_INSN(vssra,     0, v);
  DISASM_OPIV_VXI_INSN(vnsrl,     0, w);
  DISASM_OPIV_VXI_INSN(vnsra,     0, w);
  DISASM_OPIV_VXI_INSN(vnclipu,   0, w);
  DISASM_OPIV_VXI_INSN(vnclip,    0, w);

  //0b11_0000
  DISASM_OPIV_S___INSN(vwredsumu, 0);
  DISASM_OPIV_S___INSN(vwredsum,  1);
  DISASM_OPIV_V___INSN(vdotu,     0);
  DISASM_OPIV_V___INSN(vdot,      1);

  //OPMVV/OPMVX
  //0b00_0000
  DISASM_OPIV_VX__INSN(vaaddu,    0);
  DISASM_OPIV_VX__INSN(vaadd,     0);
  DISASM_OPIV_VX__INSN(vasubu,    0);
  DISASM_OPIV_VX__INSN(vasub,     0);

  DISASM_OPIV_S___INSN(vredsum,   1);
  DISASM_OPIV_S___INSN(vredand,   1);
  DISASM_OPIV_S___INSN(vredor,    1);
  DISASM_OPIV_S___INSN(vredxor,   1);
  DISASM_OPIV_S___INSN(vredminu,  0);
  DISASM_OPIV_S___INSN(vredmin,   1);
  DISASM_OPIV_S___INSN(vredmaxu,  0);
  DISASM_OPIV_S___INSN(vredmax,   1);
  DISASM_OPIV__X__INSN(vslide1up,  1);
  DISASM_OPIV__X__INSN(vslide1down,1);

  //0b01_0000
  //VWXUNARY0
  DISASM_INSN("vmv.x.s", vmv_x_s, 0, {&xrd, &vs2});
  DISASM_INSN("vpopc.m", vpopc_m, 0, {&xrd, &vs2, &opt, &vm});
  DISASM_INSN("vfirst.m", vfirst_m, 0, {&xrd, &vs2, &opt, &vm});

  //VRXUNARY0
  DISASM_INSN("vmv.s.x", vmv_s_x, 0, {&vd, &xrs1});

  //VXUNARY0
  DISASM_INSN("vzext.vf2", vzext_vf2, 0, {&vd, &vs2, &opt, &vm});
  DISASM_INSN("vsext.vf2", vsext_vf2, 0, {&vd, &vs2, &opt, &vm});
  DISASM_INSN("vzext.vf4", vzext_vf4, 0, {&vd, &vs2, &opt, &vm});
  DISASM_INSN("vsext.vf4", vsext_vf4, 0, {&vd, &vs2, &opt, &vm});
  DISASM_INSN("vzext.vf8", vzext_vf8, 0, {&vd, &vs2, &opt, &vm});
  DISASM_INSN("vsext.vf8", vsext_vf8, 0, {&vd, &vs2, &opt, &vm});

  //VMUNARY0
  DISASM_INSN("vmsbf.m", vmsbf_m, 0, {&vd, &vs2, &opt, &vm});
  DISASM_INSN("vmsof.m", vmsof_m, 0, {&vd, &vs2, &opt, &vm});
  DISASM_INSN("vmsif.m", vmsif_m, 0, {&vd, &vs2, &opt, &vm});
  DISASM_INSN("viota.m", viota_m, 0, {&vd, &vs2, &opt, &vm});
  DISASM_INSN("vid.v", vid_v, 0, {&vd, &opt, &vm});

  DISASM_INSN("vid.v", vid_v, 0, {&vd, &opt, &vm});

  DISASM_INSN("vcompress.vm", vcompress_vm, 0, {&vd, &vs2, &vs1});

  DISASM_OPIV_M___INSN(vmandnot,  1);
  DISASM_OPIV_M___INSN(vmand,     1);
  DISASM_OPIV_M___INSN(vmor,      1);
  DISASM_OPIV_M___INSN(vmxor,     1);
  DISASM_OPIV_M___INSN(vmornot,   1);
  DISASM_OPIV_M___INSN(vmnand,    1);
  DISASM_OPIV_M___INSN(vmnor,     1);
  DISASM_OPIV_M___INSN(vmxnor,    1);

  //0b10_0000
  DISASM_OPIV_VX__INSN(vdivu,     0);
  DISASM_OPIV_VX__INSN(vdiv,      1);
  DISASM_OPIV_VX__INSN(vremu,     0);
  DISASM_OPIV_VX__INSN(vrem,      1);
  DISASM_OPIV_VX__INSN(vmulhu,    0);
  DISASM_OPIV_VX__INSN(vmul,      1);
  DISASM_OPIV_VX__INSN(vmulhsu,   0);
  DISASM_OPIV_VX__INSN(vmulh,     1);
  DISASM_OPIV_VX__INSN(vmadd,     1);
  DISASM_OPIV_VX__INSN(vnmsub,    1);
  DISASM_OPIV_VX__INSN(vmacc,     1);
  DISASM_OPIV_VX__INSN(vnmsac,    1);

  //0b11_0000
  DISASM_OPIV_VX__INSN(vwaddu,    0);
  DISASM_OPIV_VX__INSN(vwadd,     1);
  DISASM_OPIV_VX__INSN(vwsubu,    0);
  DISASM_OPIV_VX__INSN(vwsub,     1);
  DISASM_OPIV_W___INSN(vwaddu,    0);
  DISASM_OPIV_W___INSN(vwadd,     1);
  DISASM_OPIV_W___INSN(vwsubu,    0);
  DISASM_OPIV_W___INSN(vwsub,     1);
  DISASM_OPIV_VX__INSN(vwmulu,    0);
  DISASM_OPIV_VX__INSN(vwmulsu,   0);
  DISASM_OPIV_VX__INSN(vwmul,     1);
  DISASM_OPIV_VX__INSN(vwmaccu,   0);
  DISASM_OPIV_VX__INSN(vwmacc,    1);
  DISASM_OPIV__X__INSN(vwmaccus,  1);
  DISASM_OPIV_VX__INSN(vwmaccsu,  0);

  #undef DISASM_OPIV_VXI_INSN
  #undef DISASM_OPIV_VX__INSN
  #undef DISASM_OPIV__XI_INSN
  #undef DISASM_OPIV_V___INSN
  #undef DISASM_OPIV_S___INSN
  #undef DISASM_OPIV_W___INSN
  #undef DISASM_OPIV_M___INSN
  #undef DISASM_OPIV__X__INSN
  #undef DISASM_OPIV_VXIM_INSN
  #undef DISASM_OPIV_VX_M_INSN

  #define DISASM_OPIV_VF_INSN(name) \
      add_insn(new disasm_insn_t(#name ".vv", match_##name##_vv, mask_##name##_vv, \
                  {&vd, &vs2, &vs1, &opt, &vm})); \
      add_insn(new disasm_insn_t(#name ".vf", match_##name##_vf, mask_##name##_vf, \
                  {&vd, &vs2, &frs1, &opt, &vm})); \

  #define DISASM_OPIV_WF_INSN(name) \
      add_insn(new disasm_insn_t(#name ".wv", match_##name##_wv, mask_##name##_wv, \
                  {&vd, &vs2, &vs1, &opt, &vm})); \
      add_insn(new disasm_insn_t(#name ".wf", match_##name##_wf, mask_##name##_wf, \
                  {&vd, &vs2, &frs1, &opt, &vm})); \

  #define DISASM_OPIV_V__INSN(name) \
      add_insn(new disasm_insn_t(#name ".vv", match_##name##_vv, mask_##name##_vv, \
                  {&vd, &vs2, &vs1, &opt, &vm}));

  #define DISASM_OPIV_S__INSN(name) \
      add_insn(new disasm_insn_t(#name ".vs", match_##name##_vs, mask_##name##_vs, \
                  {&vd, &vs2, &vs1, &opt, &vm}));

  #define DISASM_OPIV__F_INSN(name) \
    add_insn(new disasm_insn_t(#name ".vf", match_##name##_vf, mask_##name##_vf, \
                {&vd, &vs2, &frs1, &opt, &vm})); \

  #define DISASM_VFUNARY0_INSN(name, suf) \
    add_insn(new disasm_insn_t(#name "cvt.xu.f." #suf, \
                match_##name##cvt_xu_f_##suf, mask_##name##cvt_xu_f_##suf, \
                {&vd, &vs2, &opt, &vm})); \
    add_insn(new disasm_insn_t(#name "cvt.x.f." #suf, \
                match_##name##cvt_x_f_##suf, mask_##name##cvt_x_f_##suf, \
                {&vd, &vs2, &opt, &vm})); \
    add_insn(new disasm_insn_t(#name "cvt.f.xu." #suf, \
                match_##name##cvt_f_xu_##suf, mask_##name##cvt_f_xu_##suf, \
                {&vd, &vs2, &opt, &vm})); \
    add_insn(new disasm_insn_t(#name "cvt.f.x." #suf, \
                match_##name##cvt_f_x_##suf, mask_##name##cvt_f_x_##suf, \
                {&vd, &vs2, &opt, &vm})); \
    add_insn(new disasm_insn_t(#name "cvt.rtz.xu.f." #suf, \
                match_##name##cvt_rtz_xu_f_##suf, mask_##name##cvt_rtz_xu_f_##suf, \
                {&vd, &vs2, &opt, &vm})); \
    add_insn(new disasm_insn_t(#name "cvt.rtz.x.f." #suf, \
                match_##name##cvt_rtz_x_f_##suf, mask_##name##cvt_rtz_x_f_##suf, \
                {&vd, &vs2, &opt, &vm})); \

  //OPFVV/OPFVF
  //0b00_0000
  DISASM_OPIV_VF_INSN(vfadd);
  DISASM_OPIV_S__INSN(vfredsum);
  DISASM_OPIV_VF_INSN(vfsub);
  DISASM_OPIV_S__INSN(vfredosum);
  DISASM_OPIV_VF_INSN(vfmin);
  DISASM_OPIV_S__INSN(vfredmin);
  DISASM_OPIV_VF_INSN(vfmax);
  DISASM_OPIV_S__INSN(vfredmax);
  DISASM_OPIV_VF_INSN(vfsgnj);
  DISASM_OPIV_VF_INSN(vfsgnjn);
  DISASM_OPIV_VF_INSN(vfsgnjx);
  DISASM_INSN("vfmv.f.s", vfmv_f_s, 0, {&frd, &vs2});
  DISASM_INSN("vfmv.s.f", vfmv_s_f, mask_vfmv_s_f, {&vd, &frs1});
  DISASM_OPIV__F_INSN(vfslide1up);
  DISASM_OPIV__F_INSN(vfslide1down);

  //0b01_0000
  DISASM_INSN("vfmerge.vfm", vfmerge_vfm, 0, {&vd, &vs2, &frs1, &v0});
  DISASM_INSN("vfmv.v.f", vfmv_v_f, 0, {&vd, &frs1});
  DISASM_OPIV_VF_INSN(vmfeq);
  DISASM_OPIV_VF_INSN(vmfle);
  DISASM_OPIV_VF_INSN(vmflt);
  DISASM_OPIV_VF_INSN(vmfne);
  DISASM_OPIV__F_INSN(vmfgt);
  DISASM_OPIV__F_INSN(vmfge);

  //0b10_0000
  DISASM_OPIV_VF_INSN(vfdiv);
  DISASM_OPIV__F_INSN(vfrdiv);

  //vfunary0
  DISASM_VFUNARY0_INSN(vf,  v);

  DISASM_VFUNARY0_INSN(vfw, v);
  DISASM_INSN("vfwcvt.f.f.v", vfwcvt_f_f_v, 0, {&vd, &vs2, &opt, &vm});

  DISASM_VFUNARY0_INSN(vfn, w);
  DISASM_INSN("vfncvt.f.f.w", vfncvt_f_f_w, 0, {&vd, &vs2, &opt, &vm});
  DISASM_INSN("vfncvt.rod.f.f.w", vfncvt_rod_f_f_w, 0, {&vd, &vs2, &opt, &vm});

  //vfunary1
  DISASM_INSN("vfsqrt.v", vfsqrt_v, 0, {&vd, &vs2, &opt, &vm});
  DISASM_INSN("vfrsqrt7.v", vfrsqrt7_v, 0, {&vd, &vs2, &opt, &vm});
  DISASM_INSN("vfrec7.v", vfrec7_v, 0, {&vd, &vs2, &opt, &vm});
  DISASM_INSN("vfclass.v", vfclass_v, 0, {&vd, &vs2, &opt, &vm});

  DISASM_OPIV_VF_INSN(vfmul);
  DISASM_OPIV__F_INSN(vfrsub);
  DISASM_OPIV_VF_INSN(vfmadd);
  DISASM_OPIV_VF_INSN(vfnmadd);
  DISASM_OPIV_VF_INSN(vfmsub);
  DISASM_OPIV_VF_INSN(vfnmsub);
  DISASM_OPIV_VF_INSN(vfmacc);
  DISASM_OPIV_VF_INSN(vfnmacc);
  DISASM_OPIV_VF_INSN(vfmsac);
  DISASM_OPIV_VF_INSN(vfnmsac);

  //0b11_0000
  DISASM_OPIV_VF_INSN(vfwadd);
  DISASM_OPIV_S__INSN(vfwredsum);
  DISASM_OPIV_VF_INSN(vfwsub);
  DISASM_OPIV_S__INSN(vfwredosum);
  DISASM_OPIV_WF_INSN(vfwadd);
  DISASM_OPIV_WF_INSN(vfwsub);
  DISASM_OPIV_VF_INSN(vfwmul);
  DISASM_OPIV_V__INSN(vfdot);
  DISASM_OPIV_VF_INSN(vfwmacc);
  DISASM_OPIV_VF_INSN(vfwnmacc);
  DISASM_OPIV_VF_INSN(vfwmsac);
  DISASM_OPIV_VF_INSN(vfwnmsac);

  #undef DISASM_OPIV_VF_INSN
  #undef DISASM_OPIV_V__INSN
  #undef DISASM_OPIV__F_INSN
  #undef DISASM_OPIV_S__INSN
  #undef DISASM_OPIV_W__INSN
  #undef DISASM_VFUNARY0_INSN

  // vector amo
  std::vector<const arg_t *> v_fmt_amo_wd = {&vd, &v_address, &vs2, &vd, &opt, &vm};
  std::vector<const arg_t *> v_fmt_amo = {&x0, &v_address, &vs2, &vd, &opt, &vm};
  for (size_t elt = 0; elt <= 3; ++elt) {
    const custom_fmt_t template_insn[] = {
      {match_vamoaddei8_v | mask_wd,   mask_vamoaddei8_v | mask_wd,
         "%sei%d.v", v_fmt_amo_wd},
      {match_vamoaddei8_v,   mask_vamoaddei8_v | mask_wd,
         "%sei%d.v", v_fmt_amo},
    };
    std::pair<const char*, reg_t> amo_map[] = {
        {"vamoswap", 0x01ul << 27},
        {"vamoadd",  0x00ul << 27},
        {"vamoxor",  0x04ul << 27},
        {"vamoand",  0x0cul << 27},
        {"vamoor",   0x08ul << 27},
        {"vamomin",  0x10ul << 27},
        {"vamomax",  0x14ul << 27},
        {"vamominu", 0x18ul << 27},
        {"vamomaxu", 0x1cul << 27}};
    const reg_t elt_map[] = {0x0ul << 12,  0x5ul << 12,
                             0x6ul <<12, 0x7ul << 12};

    for (size_t idx = 0; idx < sizeof(amo_map) / sizeof(amo_map[0]); ++idx) {
      for (auto item : template_insn) {
        char buf[128];
        sprintf(buf, item.fmt, amo_map[idx].first, 8 << elt);
        add_insn(new disasm_insn_t(buf,
                  item.match | amo_map[idx].second | elt_map[elt],
                  item.mask,
                  item.arg));
      }
    }
  }

#define DEFINE_PITYTPE(code, immbit) DISASM_INSN(#code, code, 0, {&xrd, &xrs1, &p_imm##immbit});
#define DEFINE_ONEOP(code) DISASM_INSN(#code, code, 0, {&xrd, &xrs1});

#define DISASM_8_AND_16_RINSN(code) \
  DEFINE_RTYPE(code##8); \
  DEFINE_RTYPE(code##16);

#define DISASM_8_AND_16_RINSN_ROUND(code) \
  DISASM_INSN(#code "8.u", code##8_u, 0, {&xrd, &xrs1, &xrs2}); \
  DISASM_INSN(#code "16.u", code##16_u, 0, {&xrd, &xrs1, &xrs2}); \

#define DISASM_8_AND_16_PIINSN(code) \
  DEFINE_PITYTPE(code##8, 3); \
  DEFINE_PITYTPE(code##16, 4);

#define DISASM_8_AND_16_PIINSN_ROUND(code) \
  DISASM_INSN(#code "8.u", code##8_u, 0, {&xrd, &xrs1, &p_imm3}); \
  DISASM_INSN(#code "16.u", code##16_u, 0, {&xrd, &xrs1, &p_imm4});

#define DISASM_RINSN_AND_ROUND(code) \
  DEFINE_RTYPE(code); \
  DISASM_INSN(#code ".u", code##_u, 0, {&xrd, &xrs1, &xrs2});

  DISASM_8_AND_16_RINSN(add);
  DISASM_8_AND_16_RINSN(radd);
  DISASM_8_AND_16_RINSN(uradd);
  DISASM_8_AND_16_RINSN(kadd);
  DISASM_8_AND_16_RINSN(ukadd);
  DISASM_8_AND_16_RINSN(sub);
  DISASM_8_AND_16_RINSN(rsub);
  DISASM_8_AND_16_RINSN(ursub);
  DISASM_8_AND_16_RINSN(ksub);
  DISASM_8_AND_16_RINSN(uksub);
  DEFINE_RTYPE(cras16);
  DEFINE_RTYPE(rcras16);
  DEFINE_RTYPE(urcras16);
  DEFINE_RTYPE(kcras16);
  DEFINE_RTYPE(ukcras16);
  DEFINE_RTYPE(crsa16);
  DEFINE_RTYPE(rcrsa16);
  DEFINE_RTYPE(urcrsa16);
  DEFINE_RTYPE(kcrsa16);
  DEFINE_RTYPE(ukcrsa16);
  DEFINE_RTYPE(stas16);
  DEFINE_RTYPE(rstas16);
  DEFINE_RTYPE(urstas16);
  DEFINE_RTYPE(kstas16);
  DEFINE_RTYPE(ukstas16);
  DEFINE_RTYPE(stsa16);
  DEFINE_RTYPE(rstsa16);
  DEFINE_RTYPE(urstsa16);
  DEFINE_RTYPE(kstsa16);
  DEFINE_RTYPE(ukstsa16);

  DISASM_8_AND_16_RINSN(sra);
  DISASM_8_AND_16_RINSN(srl);
  DISASM_8_AND_16_RINSN(sll);
  DISASM_8_AND_16_RINSN(ksll);
  DISASM_8_AND_16_RINSN(kslra);
  DISASM_8_AND_16_PIINSN(srai);
  DISASM_8_AND_16_PIINSN(srli);
  DISASM_8_AND_16_PIINSN(slli);
  DISASM_8_AND_16_PIINSN(kslli);
  DISASM_8_AND_16_RINSN_ROUND(sra);
  DISASM_8_AND_16_RINSN_ROUND(srl);
  DISASM_8_AND_16_RINSN_ROUND(kslra);
  DISASM_8_AND_16_PIINSN_ROUND(srai);
  DISASM_8_AND_16_PIINSN_ROUND(srli);

  DISASM_8_AND_16_RINSN(cmpeq);
  DISASM_8_AND_16_RINSN(scmplt);
  DISASM_8_AND_16_RINSN(scmple);
  DISASM_8_AND_16_RINSN(ucmplt);
  DISASM_8_AND_16_RINSN(ucmple);

  DISASM_8_AND_16_RINSN(smul);
  DISASM_8_AND_16_RINSN(smulx);
  DISASM_8_AND_16_RINSN(umul);
  DISASM_8_AND_16_RINSN(umulx);
  DISASM_8_AND_16_RINSN(khm);
  DISASM_8_AND_16_RINSN(khmx);

  DISASM_8_AND_16_RINSN(smin);
  DISASM_8_AND_16_RINSN(umin);
  DISASM_8_AND_16_RINSN(smax);
  DISASM_8_AND_16_RINSN(umax);
  DISASM_8_AND_16_PIINSN(sclip);
  DISASM_8_AND_16_PIINSN(uclip);
  DEFINE_ONEOP(kabs16);
  DEFINE_ONEOP(clrs16);
  DEFINE_ONEOP(clz16);
  DEFINE_ONEOP(clo16);
  DEFINE_ONEOP(swap16);
  DEFINE_ONEOP(kabs8);
  DEFINE_ONEOP(clrs8);
  DEFINE_ONEOP(clz8);
  DEFINE_ONEOP(clo8);
  DEFINE_ONEOP(swap8);

  DEFINE_ONEOP(sunpkd810);
  DEFINE_ONEOP(sunpkd820);
  DEFINE_ONEOP(sunpkd830);
  DEFINE_ONEOP(sunpkd831);
  DEFINE_ONEOP(sunpkd832);
  DEFINE_ONEOP(zunpkd810);
  DEFINE_ONEOP(zunpkd820);
  DEFINE_ONEOP(zunpkd830);
  DEFINE_ONEOP(zunpkd831);
  DEFINE_ONEOP(zunpkd832);

  DEFINE_RTYPE(pkbb16);
  DEFINE_RTYPE(pkbt16);
  DEFINE_RTYPE(pktb16);
  DEFINE_RTYPE(pktt16);
  DISASM_RINSN_AND_ROUND(smmul);
  DISASM_RINSN_AND_ROUND(kmmac);
  DISASM_RINSN_AND_ROUND(kmmsb);
  DISASM_RINSN_AND_ROUND(kwmmul);
  DISASM_RINSN_AND_ROUND(smmwb);
  DISASM_RINSN_AND_ROUND(smmwt);
  DISASM_RINSN_AND_ROUND(kmmawb);
  DISASM_RINSN_AND_ROUND(kmmawt);
  DISASM_RINSN_AND_ROUND(kmmwb2);
  DISASM_RINSN_AND_ROUND(kmmwt2);
  DISASM_RINSN_AND_ROUND(kmmawb2);
  DISASM_RINSN_AND_ROUND(kmmawt2);
  DEFINE_RTYPE(smbb16)
  DEFINE_RTYPE(smbt16)
  DEFINE_RTYPE(smtt16)
  DEFINE_RTYPE(kmda)
  DEFINE_RTYPE(kmxda)
  DEFINE_RTYPE(smds)
  DEFINE_RTYPE(smdrs)
  DEFINE_RTYPE(smxds)
  DEFINE_RTYPE(kmabb)
  DEFINE_RTYPE(kmabt)
  DEFINE_RTYPE(kmatt)
  DEFINE_RTYPE(kmada)
  DEFINE_RTYPE(kmaxda)
  DEFINE_RTYPE(kmads)
  DEFINE_RTYPE(kmadrs)
  DEFINE_RTYPE(kmaxds)
  DEFINE_RTYPE(kmsda)
  DEFINE_RTYPE(kmsxda)
  DEFINE_RTYPE(smal)
  DEFINE_RTYPE(sclip32)
  DEFINE_RTYPE(uclip32)
  DEFINE_ONEOP(clrs32);
  DEFINE_ONEOP(clz32);
  DEFINE_ONEOP(clo32);
  DEFINE_RTYPE(pbsad);
  DEFINE_RTYPE(pbsada);
  DEFINE_RTYPE(smaqa);
  DEFINE_RTYPE(umaqa);
  DISASM_INSN("smaqa.su", smaqa_su, 0, {&xrd, &xrs1, &xrs2});

  DEFINE_RTYPE(add64);
  DEFINE_RTYPE(radd64);
  DEFINE_RTYPE(uradd64);
  DEFINE_RTYPE(kadd64);
  DEFINE_RTYPE(ukadd64);
  DEFINE_RTYPE(sub64);
  DEFINE_RTYPE(rsub64);
  DEFINE_RTYPE(ursub64);
  DEFINE_RTYPE(ksub64);
  DEFINE_RTYPE(uksub64);
  DEFINE_RTYPE(smar64);
  DEFINE_RTYPE(smsr64);
  DEFINE_RTYPE(umar64);
  DEFINE_RTYPE(umsr64);
  DEFINE_RTYPE(kmar64);
  DEFINE_RTYPE(kmsr64);
  DEFINE_RTYPE(ukmar64);
  DEFINE_RTYPE(ukmsr64);
  DEFINE_RTYPE(smalbb);
  DEFINE_RTYPE(smalbt);
  DEFINE_RTYPE(smaltt);
  DEFINE_RTYPE(smalda);
  DEFINE_RTYPE(smalxda);
  DEFINE_RTYPE(smalds);
  DEFINE_RTYPE(smaldrs);
  DEFINE_RTYPE(smalxds);
  DEFINE_RTYPE(smslda);
  DEFINE_RTYPE(smslxda);

  DEFINE_RTYPE(kaddh);
  DEFINE_RTYPE(ksubh);
  DEFINE_RTYPE(khmbb);
  DEFINE_RTYPE(khmbt);
  DEFINE_RTYPE(khmtt);
  DEFINE_RTYPE(ukaddh);
  DEFINE_RTYPE(uksubh);
  DEFINE_RTYPE(kaddw);
  DEFINE_RTYPE(ukaddw);
  DEFINE_RTYPE(ksubw);
  DEFINE_RTYPE(uksubw);
  DEFINE_RTYPE(kdmbb);
  DEFINE_RTYPE(kdmbt);
  DEFINE_RTYPE(kdmtt);
  DEFINE_RTYPE(kslraw);
  DISASM_INSN("kslraw.u", kslraw_u, 0, {&xrd, &xrs1, &xrs2});
  DEFINE_RTYPE(ksllw);
  DEFINE_PITYTPE(kslliw, 5);
  DEFINE_RTYPE(kdmabb);
  DEFINE_RTYPE(kdmabt);
  DEFINE_RTYPE(kdmatt);
  DEFINE_RTYPE(kabsw);
  DEFINE_RTYPE(raddw);
  DEFINE_RTYPE(uraddw);
  DEFINE_RTYPE(rsubw);
  DEFINE_RTYPE(ursubw);
  DEFINE_RTYPE(maxw);
  DEFINE_RTYPE(minw);
  DEFINE_RTYPE(mulr64);
  DEFINE_RTYPE(mulsr64);
  DEFINE_RTYPE(msubr32);
  DEFINE_RTYPE(ave);
  DISASM_INSN("sra.u", sra_u, 0, {&xrd, &xrs1, &xrs2});
  DISASM_INSN("srai.u", srai_u, 0, {&xrd, &xrs1, &p_imm5});
  DEFINE_RTYPE(bitrev);
  DEFINE_RTYPE(wext);
  DEFINE_PITYTPE(wexti, 5);
  DISASM_INSN("bpick", bpick, 0, {&xrd, &xrs1, &xrs2, &xrs3});
  DEFINE_PITYTPE(insb, 3);
  DEFINE_RTYPE(maddr32)

  if (xlen == 32) {
    DISASM_INSN("c.flw", c_flw, 0, {&rvc_fp_rs2s, &rvc_lw_address});
    DISASM_INSN("c.flwsp", c_flwsp, 0, {&frd, &rvc_lwsp_address});
    DISASM_INSN("c.fsw", c_fsw, 0, {&rvc_fp_rs2s, &rvc_lw_address});
    DISASM_INSN("c.fswsp", c_fswsp, 0, {&rvc_fp_rs2, &rvc_swsp_address});
    DISASM_INSN("c.jal", c_jal, 0, {&rvc_jump_target});
    DEFINE_PITYTPE(bitrevi, 5);
  } else {
    DISASM_INSN("c.ld", c_ld, 0, {&rvc_rs2s, &rvc_ld_address});
    DISASM_INSN("c.ldsp", c_ldsp, 0, {&xrd, &rvc_ldsp_address});
    DISASM_INSN("c.sd", c_sd, 0, {&rvc_rs2s, &rvc_ld_address});
    DISASM_INSN("c.sdsp", c_sdsp, 0, {&rvc_rs2, &rvc_sdsp_address});
    DISASM_INSN("c.addiw", c_addiw, 0, {&xrd, &rvc_imm});
    DEFINE_PITYTPE(bitrevi, 6);
    DEFINE_RTYPE(add32);
    DEFINE_RTYPE(radd32);
    DEFINE_RTYPE(uradd32);
    DEFINE_RTYPE(kadd32);
    DEFINE_RTYPE(ukadd32);
    DEFINE_RTYPE(sub32);
    DEFINE_RTYPE(rsub32);
    DEFINE_RTYPE(ursub32);
    DEFINE_RTYPE(ksub32);
    DEFINE_RTYPE(uksub32);
    DEFINE_RTYPE(cras32);
    DEFINE_RTYPE(rcras32);
    DEFINE_RTYPE(urcras32);
    DEFINE_RTYPE(kcras32);
    DEFINE_RTYPE(ukcras32);
    DEFINE_RTYPE(crsa32);
    DEFINE_RTYPE(rcrsa32);
    DEFINE_RTYPE(urcrsa32);
    DEFINE_RTYPE(kcrsa32);
    DEFINE_RTYPE(ukcrsa32);
    DEFINE_RTYPE(stas32);
    DEFINE_RTYPE(rstas32);
    DEFINE_RTYPE(urstas32);
    DEFINE_RTYPE(kstas32);
    DEFINE_RTYPE(ukstas32);
    DEFINE_RTYPE(stsa32);
    DEFINE_RTYPE(rstsa32);
    DEFINE_RTYPE(urstsa32);
    DEFINE_RTYPE(kstsa32);
    DEFINE_RTYPE(ukstsa32);
    DEFINE_RTYPE(sra32);
    DEFINE_PITYTPE(srai32, 5);
    DISASM_INSN("sra32.u", sra32_u, 0, {&xrd, &xrs1, &xrs2});
    DISASM_INSN("srai32.u", srai32_u, 0, {&xrd, &xrs1, &p_imm5});
    DEFINE_RTYPE(srl32);
    DEFINE_PITYTPE(srli32, 5);
    DISASM_INSN("srl32.u", srl32_u, 0, {&xrd, &xrs1, &xrs2});
    DISASM_INSN("srli32.u", srli32_u, 0, {&xrd, &xrs1, &p_imm5});
    DEFINE_RTYPE(sll32);
    DEFINE_PITYTPE(slli32, 5);
    DEFINE_RTYPE(ksll32);
    DEFINE_PITYTPE(kslli32, 5);
    DEFINE_RTYPE(kslra32);
    DISASM_INSN("kslra32.u", kslra32_u, 0, {&xrd, &xrs1, &xrs2});
    DEFINE_RTYPE(smin32);
    DEFINE_RTYPE(umin32);
    DEFINE_RTYPE(smax32);
    DEFINE_RTYPE(umax32);
    DEFINE_ONEOP(kabs32);
    DEFINE_RTYPE(khmbb16);
    DEFINE_RTYPE(khmbt16);
    DEFINE_RTYPE(khmtt16);
    DEFINE_RTYPE(kdmbb16);
    DEFINE_RTYPE(kdmbt16);
    DEFINE_RTYPE(kdmtt16);
    DEFINE_RTYPE(kdmabb16);
    DEFINE_RTYPE(kdmabt16);
    DEFINE_RTYPE(kdmatt16);
    DEFINE_RTYPE(smbt32);
    DEFINE_RTYPE(smtt32);
    DEFINE_RTYPE(kmabb32);
    DEFINE_RTYPE(kmabt32);
    DEFINE_RTYPE(kmatt32);
    DEFINE_RTYPE(kmda32);
    DEFINE_RTYPE(kmxda32);
    DEFINE_RTYPE(kmaxda32);
    DEFINE_RTYPE(kmads32);
    DEFINE_RTYPE(kmadrs32);
    DEFINE_RTYPE(kmaxds32);
    DEFINE_RTYPE(kmsda32);
    DEFINE_RTYPE(kmsxda32);
    DEFINE_RTYPE(smds32);
    DEFINE_RTYPE(smdrs32);
    DEFINE_RTYPE(smxds32);
    DISASM_INSN("sraiw.u", sraiw_u, 0, {&xrd, &xrs1, &p_imm5});
    DEFINE_RTYPE(pkbb32);
    DEFINE_RTYPE(pkbt32);
    DEFINE_RTYPE(pktb32);
    DEFINE_RTYPE(pktt32);
  }

  // provide a default disassembly for all instructions as a fallback
  #define DECLARE_INSN(code, match, mask) \
   add_insn(new disasm_insn_t(#code " (args unknown)", match, mask, {}));
  #define DECLARE_RV32_ONLY(code) {}
  #define DECLARE_RV64_ONLY(code) {}
  #include "encoding.h"
  #undef DECLARE_RV64_INSN
  #undef DECLARE_RV32_INSN
  #undef DECLARE_INSN
}

const disasm_insn_t* disassembler_t::lookup(insn_t insn) const
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

void NOINLINE disassembler_t::add_insn(disasm_insn_t* insn)
{
  size_t idx = HASH_SIZE;
  if (insn->get_mask() % HASH_SIZE == HASH_SIZE - 1)
    idx = insn->get_match() % HASH_SIZE;
  chain[idx].push_back(insn);
}

disassembler_t::~disassembler_t()
{
  for (size_t i = 0; i < HASH_SIZE+1; i++)
    for (size_t j = 0; j < chain[i].size(); j++)
      delete chain[i][j];
}
