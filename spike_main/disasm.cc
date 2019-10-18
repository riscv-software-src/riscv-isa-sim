// See LICENSE for license details.

#include "disasm.h"
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
    s << "e" << sew;
    if (lmul != 1)
      s << ",m" << lmul;
    return s.str();
  }
} v_vtype;


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
  const uint32_t match_imm_1 = 1UL << 20;
  const uint32_t mask_rvc_rs2 = 0x1fUL << 2;
  const uint32_t mask_rvc_imm = mask_rvc_rs2 | 0x1000UL;
  const uint32_t mask_nf = 0x7Ul << 29;

  #define DECLARE_INSN(code, match, mask) \
   const uint32_t match_##code = match; \
   const uint32_t mask_##code = mask;
  #include "encoding.h"
  #undef DECLARE_INSN

  // explicit per-instruction disassembly
  #define DISASM_INSN(name, code, extra, ...) \
    add_insn(new disasm_insn_t(name, match_##code, mask_##code | (extra), __VA_ARGS__));
  #define DEFINE_NOARG(code) \
    add_insn(new disasm_insn_t(#code, match_##code, mask_##code, {}));
  #define DEFINE_RTYPE(code) DISASM_INSN(#code, code, 0, {&xrd, &xrs1, &xrs2})
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
  DEFINE_FLOAD(flq)

  DEFINE_FSTORE(fsw)
  DEFINE_FSTORE(fsd)
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
  add_insn(new disasm_insn_t(" - ", match_xor, mask_xor | mask_rd | mask_rs1 | mask_rs2, {})); // for machine-generated bubbles
  DEFINE_I0TYPE("li", addi);
  DEFINE_I1TYPE("mv", addi);
  DEFINE_ITYPE(addi);
  DEFINE_ITYPE(slti);
  add_insn(new disasm_insn_t("seqz", match_sltiu | match_imm_1, mask_sltiu | mask_imm, {&xrd, &xrs1}));
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

  DEFINE_NOARG(ecall);
  DEFINE_NOARG(ebreak);
  DEFINE_NOARG(uret);
  DEFINE_NOARG(sret);
  DEFINE_NOARG(mret);
  DEFINE_NOARG(dret);
  DEFINE_NOARG(wfi);
  DEFINE_NOARG(fence);
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
  DEFINE_XFTYPE(fmv_q_x);
  DEFINE_FXTYPE(fcvt_l_q);
  DEFINE_FXTYPE(fcvt_lu_q);
  DEFINE_FXTYPE(fcvt_w_q);
  DEFINE_FXTYPE(fcvt_wu_q);
  DEFINE_FXTYPE(fclass_q);
  DEFINE_FXTYPE(fmv_x_q);
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

  DISASM_INSN("vsetvli", vsetvli, 0, {&xrd, &xrs1, &v_vtype});
  DISASM_INSN("vsetvl", vsetvl, 0, {&xrd, &xrs1, &xrs2});

  #define DISASM_VMEM_LD_INSN(name, ff, fmt) \
    add_insn(new disasm_insn_t("vl" #name "b" #ff ".v",  match_vl##name##b##ff##_v,  mask_vl##name##b##ff##_v | mask_nf, fmt)); \
    add_insn(new disasm_insn_t("vl" #name "h" #ff ".v",  match_vl##name##h##ff##_v,  mask_vl##name##h##ff##_v | mask_nf, fmt)); \
    add_insn(new disasm_insn_t("vl" #name "w" #ff ".v",  match_vl##name##w##ff##_v,  mask_vl##name##w##ff##_v | mask_nf, fmt)); \
    add_insn(new disasm_insn_t("vl" #name "e" #ff ".v",  match_vl##name##e##ff##_v,  mask_vl##name##e##ff##_v | mask_nf, fmt)); \
    add_insn(new disasm_insn_t("vl" #name "bu" #ff ".v", match_vl##name##bu##ff##_v, mask_vl##name##bu##ff##_v | mask_nf, fmt)); \
    add_insn(new disasm_insn_t("vl" #name "hu" #ff ".v", match_vl##name##hu##ff##_v, mask_vl##name##hu##ff##_v | mask_nf, fmt)); \
    add_insn(new disasm_insn_t("vl" #name "wu" #ff ".v", match_vl##name##wu##ff##_v, mask_vl##name##wu##ff##_v | mask_nf, fmt));

  #define DISASM_VMEM_ST_INSN(name, fmt) \
    add_insn(new disasm_insn_t("vs" #name "b.v", match_vs##name##b_v, mask_vs##name##b_v | mask_nf, fmt)); \
    add_insn(new disasm_insn_t("vs" #name "h.v", match_vs##name##h_v, mask_vs##name##h_v | mask_nf, fmt)); \
    add_insn(new disasm_insn_t("vs" #name "w.v", match_vs##name##w_v, mask_vs##name##w_v | mask_nf, fmt)); \
    add_insn(new disasm_insn_t("vs" #name "e.v", match_vs##name##e_v, mask_vs##name##e_v | mask_nf, fmt));

  const std::vector<const arg_t *> v_ld_unit = {&vd, &v_address, &opt, &vm};
  const std::vector<const arg_t *> v_st_unit = {&vs3, &v_address, &opt, &vm};
  const std::vector<const arg_t *> v_ld_stride = {&vd, &v_address, &xrs2, &opt, &vm};
  const std::vector<const arg_t *> v_st_stride = {&vs3, &v_address, &xrs2, &opt, &vm};
  const std::vector<const arg_t *> v_ld_index = {&vd, &v_address, &vs2, &opt, &vm};
  const std::vector<const arg_t *> v_st_index = {&vs3, &v_address, &vs2, &opt, &vm};

  DISASM_VMEM_LD_INSN( ,   , v_ld_unit);
  DISASM_VMEM_ST_INSN( ,     v_st_unit);
  DISASM_VMEM_LD_INSN(s,   , v_ld_stride);
  DISASM_VMEM_ST_INSN(s,     v_st_stride);
  DISASM_VMEM_LD_INSN(x,   , v_ld_index);
  DISASM_VMEM_ST_INSN(x,     v_st_index);
  DISASM_VMEM_LD_INSN( , ff, v_ld_unit);

  #undef DISASM_VMEM_LD_INSN
  #undef DISASM_VMEM_ST_INSN

  // handle vector segment load/store
  for (size_t nf = 1; nf <= 7; ++nf) {
    std::pair<reg_t, reg_t> insn_code[] = {
      {match_vlb_v,  mask_vlb_v},
      {match_vlh_v,  mask_vlh_v},
      {match_vlw_v,  mask_vlw_v},
      {match_vle_v,  mask_vle_v},
      {match_vlbu_v, mask_vlbu_v},
      {match_vlhu_v, mask_vlhu_v},
      {match_vlwu_v, mask_vlwu_v},
      {match_vsb_v,  mask_vsb_v},
      {match_vsh_v,  mask_vsh_v},
      {match_vsw_v,  mask_vsw_v},
      {match_vse_v,  mask_vse_v},

      {match_vlsb_v,  mask_vlsb_v},
      {match_vlsh_v,  mask_vlsh_v},
      {match_vlsw_v,  mask_vlsw_v},
      {match_vlse_v,  mask_vlse_v},
      {match_vlsbu_v, mask_vlsbu_v},
      {match_vlshu_v, mask_vlshu_v},
      {match_vlswu_v, mask_vlswu_v},
      {match_vssb_v,  mask_vssb_v},
      {match_vssh_v,  mask_vssh_v},
      {match_vssw_v,  mask_vssw_v},
      {match_vsse_v,  mask_vssw_v},

      {match_vlxb_v,  mask_vlxb_v},
      {match_vlxh_v,  mask_vlxh_v},
      {match_vlxw_v,  mask_vlxw_v},
      {match_vlxe_v,  mask_vlxe_v},
      {match_vlxbu_v, mask_vlxbu_v},
      {match_vlxhu_v, mask_vlxhu_v},
      {match_vlxwu_v, mask_vlxwu_v},
      {match_vsxb_v,  mask_vsxb_v},
      {match_vsxh_v,  mask_vsxh_v},
      {match_vsxw_v,  mask_vsxw_v},
      {match_vsxe_v,  mask_vsxw_v},

      {match_vlbff_v,  mask_vlbff_v},
      {match_vlhff_v,  mask_vlhff_v},
      {match_vlwff_v,  mask_vlwff_v},
      {match_vleff_v,  mask_vleff_v},
      {match_vlbuff_v, mask_vlbuff_v},
      {match_vlhuff_v, mask_vlhuff_v},
      {match_vlwuff_v, mask_vlwuff_v},
    };

    std::pair<const char *, std::vector<const arg_t*>> fmts[] = { 
      {"vlseg%db.v", {&vd, &v_address, &opt, &vm}},
      {"vlseg%dh.v", {&vd, &v_address, &opt, &vm}},
      {"vlseg%dw.v", {&vd, &v_address, &opt, &vm}},
      {"vlseg%de.v", {&vd, &v_address, &opt, &vm}},
      {"vlseg%dwu.v", {&vd, &v_address, &opt, &vm}},
      {"vlseg%dhu.v", {&vd, &v_address, &opt, &vm}},
      {"vlseg%dbu.v", {&vd, &v_address, &opt, &vm}},
      {"vsseg%db.v", {&vs3, &v_address, &opt, &vm}},
      {"vsseg%dh.v", {&vs3, &v_address, &opt, &vm}},
      {"vsseg%dw.v", {&vs3, &v_address, &opt, &vm}},
      {"vsseg%de.v", {&vs3, &v_address, &opt, &vm}},

      {"vlsseg%db.v", {&vd, &v_address, &xrs2, &opt, &vm}},
      {"vlsseg%dh.v", {&vd, &v_address, &xrs2, &opt, &vm}},
      {"vlsseg%dw.v", {&vd, &v_address, &xrs2, &opt, &vm}},
      {"vlsseg%de.v", {&vd, &v_address, &xrs2, &opt, &vm}},
      {"vlsseg%dbu.v",{&vd, &v_address, &xrs2, &opt, &vm}},
      {"vlsseg%dhu.v",{&vd, &v_address, &xrs2, &opt, &vm}},
      {"vlsseg%dwu.v",{&vd, &v_address, &xrs2, &opt, &vm}},
      {"vssseg%db.v", {&vs3, &v_address, &xrs2, &opt, &vm}},
      {"vssseg%dh.v", {&vs3, &v_address, &xrs2, &opt, &vm}},
      {"vssseg%dw.v", {&vs3, &v_address, &xrs2, &opt, &vm}},
      {"vssseg%de.v", {&vs3, &v_address, &xrs2, &opt, &vm}},

      {"vlseg%db.v", {&vd, &v_address, &vs2, &opt, &vm}},
      {"vlseg%dh.v", {&vd, &v_address, &vs2, &opt, &vm}},
      {"vlseg%dw.v", {&vd, &v_address, &vs2, &opt, &vm}},
      {"vlseg%de.v", {&vd, &v_address, &vs2, &opt, &vm}},
      {"vlseg%dwu.v",{&vd, &v_address, &vs2, &opt, &vm}},
      {"vlseg%dhu.v",{&vd, &v_address, &vs2, &opt, &vm}},
      {"vlseg%dbu.v",{&vd, &v_address, &vs2, &opt, &vm}},
      {"vsseg%db.v", {&vs3, &v_address, &vs2, &opt, &vm}},
      {"vsseg%dh.v", {&vs3, &v_address, &vs2, &opt, &vm}},
      {"vsseg%dw.v", {&vs3, &v_address, &vs2, &opt, &vm}},
      {"vsseg%de.v", {&vs3, &v_address, &vs2, &opt, &vm}},

      {"vlseg%dbff.v", {&vd, &v_address, &opt, &vm}},
      {"vlseg%dhff.v", {&vd, &v_address, &opt, &vm}},
      {"vlseg%dwff.v", {&vd, &v_address, &opt, &vm}},
      {"vlseg%deff.v", {&vd, &v_address, &opt, &vm}},
      {"vlseg%dwuff.v",{&vd, &v_address, &opt, &vm}},
      {"vlseg%dhuff.v",{&vd, &v_address, &opt, &vm}},
      {"vlseg%dbuff.v",{&vd, &v_address, &opt, &vm}},
    };

    for (size_t idx_insn = 0; idx_insn < sizeof(insn_code) / sizeof(insn_code[0]); ++idx_insn) {
      const reg_t match_nf = nf << 29;
      char buf[128];
      sprintf(buf, fmts[idx_insn].first, nf + 1);
      add_insn(new disasm_insn_t(buf,
                                 insn_code[idx_insn].first | match_nf,
                                 insn_code[idx_insn].second | mask_nf,
                                 fmts[idx_insn].second
                                 ));
    }
  }


  #define DISASM_OPIV_VXI_INSN(name, sign) \
    add_insn(new disasm_insn_t(#name ".vv", match_##name##_vv, mask_##name##_vv, \
                {&vd, &vs2, &vs1, &opt, &vm})); \
    add_insn(new disasm_insn_t(#name ".vx", match_##name##_vx, mask_##name##_vx, \
                {&vd, &vs2, &xrs1, &opt, &vm})); \
    if (sign) \
    add_insn(new disasm_insn_t(#name ".vi", match_##name##_vi, mask_##name##_vi, \
                {&vd, &vs2, &v_simm5, &opt, &vm})); \
    else \
    add_insn(new disasm_insn_t(#name ".vi", match_##name##_vi, mask_##name##_vi, \
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
                {&vd, &vs2, &vs1, &opt, &vm}));

  #define DISASM_OPIV_M___INSN(name, sign) \
    add_insn(new disasm_insn_t(#name ".mm", match_##name##_mm, mask_##name##_mm, \
                {&vd, &vs2, &vs1}));

  #define DISASM_OPIV__X__INSN(name, sign) \
    add_insn(new disasm_insn_t(#name ".vx", match_##name##_vx, mask_##name##_vx, \
                {&vd, &vs2, &xrs1, &opt, &vm}));

  #define DISASM_OPIV_VXIM_INSN(name, sign) \
    add_insn(new disasm_insn_t(#name ".vvm", match_##name##_vvm, mask_##name##_vvm, \
                {&vd, &vs2, &vs1, &v0})); \
    add_insn(new disasm_insn_t(#name ".vxm", match_##name##_vxm, mask_##name##_vxm, \
                {&vd, &vs2, &xrs1, &v0})); \
    add_insn(new disasm_insn_t(#name ".vim", match_##name##_vim, mask_##name##_vim, \
                {&vd, &vs2, &v_simm5, &v0}));

  #define DISASM_OPIV_VX_M_INSN(name, sign) \
    add_insn(new disasm_insn_t(#name ".vvm", match_##name##_vvm, mask_##name##_vvm, \
                {&vd, &vs2, &vs1, &v0})); \
    add_insn(new disasm_insn_t(#name ".vxm", match_##name##_vxm, mask_##name##_vxm, \
                {&vd, &vs2, &xrs1, &v0}));

  //OPFVV/OPFVF
  //0b00_0000
  DISASM_OPIV_VXI_INSN(vadd,      1);
  DISASM_OPIV_VX__INSN(vsub,      1);
  DISASM_OPIV__XI_INSN(vrsub,     1);
  DISASM_OPIV_VX__INSN(vminu,     0);
  DISASM_OPIV_VX__INSN(vmin,      1);
  DISASM_OPIV_VX__INSN(vmaxu,     1);
  DISASM_OPIV_VX__INSN(vmax,      0);
  DISASM_OPIV_VXI_INSN(vand,      1);
  DISASM_OPIV_VXI_INSN(vor,       1);
  DISASM_OPIV_VXI_INSN(vxor,      1);
  DISASM_OPIV_VXI_INSN(vrgather,  0);
  DISASM_OPIV__XI_INSN(vslideup,  1);
  DISASM_OPIV__XI_INSN(vslidedown,1);

  //0b01_0000
  DISASM_OPIV_VXIM_INSN(vadc,    1);
  DISASM_OPIV_VXIM_INSN(vmadc,   1);
  DISASM_OPIV_VX_M_INSN(vsbc,    1);
  DISASM_OPIV_VX_M_INSN(vmsbc,   1);
  DISASM_OPIV_VXIM_INSN(vmerge,  1);
  DISASM_INSN("vmv.v.i", vmv_v_i, 0, {&vd, &v_simm5});
  DISASM_INSN("vmv.v.v", vmv_v_v, 0, {&vd, &vs1});
  DISASM_INSN("vmv.v.x", vmv_v_x, 0, {&vd, &xrs1});
  DISASM_OPIV_VXI_INSN(vmseq,     1);
  DISASM_OPIV_VXI_INSN(vmsne,     1);
  DISASM_OPIV_VX__INSN(vmsltu,    0);
  DISASM_OPIV_VX__INSN(vmslt,     1);
  DISASM_OPIV_VXI_INSN(vmsleu,    0);
  DISASM_OPIV_VXI_INSN(vmsle,     1);
  DISASM_OPIV__XI_INSN(vmsgtu,    0);
  DISASM_OPIV__XI_INSN(vmsgt,     1);

  //0b10_0000
  DISASM_OPIV_VXI_INSN(vsaddu,    0);
  DISASM_OPIV_VXI_INSN(vsadd,     1);
  DISASM_OPIV_VX__INSN(vssubu,    0);
  DISASM_OPIV_VX__INSN(vssub,     1);
  DISASM_OPIV_VXI_INSN(vaadd,     1);
  DISASM_OPIV_VXI_INSN(vsll,      1);
  DISASM_OPIV_VX__INSN(vasub,     1);
  DISASM_OPIV_VX__INSN(vsmul,     1);
  DISASM_OPIV_VXI_INSN(vsrl,      0);
  DISASM_OPIV_VXI_INSN(vsra,      0);
  DISASM_OPIV_VXI_INSN(vssrl,     0);
  DISASM_OPIV_VXI_INSN(vssra,     0);
  DISASM_OPIV_VXI_INSN(vnsrl,     0);
  DISASM_OPIV_VXI_INSN(vnsra,     0);
  DISASM_OPIV_VXI_INSN(vnclipu,   0);
  DISASM_OPIV_VXI_INSN(vnclip,    1);

  //0b11_0000
  DISASM_OPIV_S___INSN(vwredsumu, 0);
  DISASM_OPIV_S___INSN(vwredsum,  1);
  DISASM_OPIV_V___INSN(vdotu,     0);
  DISASM_OPIV_V___INSN(vdot,      1);
  DISASM_OPIV_VX__INSN(vwsmaccu,  0);
  DISASM_OPIV_VX__INSN(vwsmacc,   1);
  DISASM_OPIV_VX__INSN(vwsmaccsu, 0);
  DISASM_OPIV__X__INSN(vwsmaccus, 1);

  //OPMVV/OPMVX
  //0b00_0000
  DISASM_OPIV_S___INSN(vredsum,   1);
  DISASM_OPIV_S___INSN(vredand,   1);
  DISASM_OPIV_S___INSN(vredor,    1);
  DISASM_OPIV_S___INSN(vredxor,   1);
  DISASM_OPIV_S___INSN(vredminu,  0);
  DISASM_OPIV_S___INSN(vredmin,   1);
  DISASM_OPIV_S___INSN(vredmaxu,  0);
  DISASM_OPIV_S___INSN(vredmax,   1);
  DISASM_INSN("vmv.x.s", vmv_x_s, 0, {&xrd, &vs2});
  DISASM_INSN("vmv.s.x", vmv_s_x, 0, {&vd, &xrs1});
  DISASM_OPIV__X__INSN(vslide1up,  1);
  DISASM_OPIV__X__INSN(vslide1down,1);

  //0b01_0000
  DISASM_INSN("vpopc.m", vpopc_m, 0, {&xrd, &vs2, &opt, &vm});
  //vmuary0
  DISASM_INSN("vfirst.m", vfirst_m, 0, {&xrd, &vs2, &opt, &vm});
  DISASM_INSN("vmsbf.m", vmsbf_m, 0, {&vd, &vs2, &opt, &vm});
  DISASM_INSN("vmsof.m", vmsof_m, 0, {&vd, &vs2, &opt, &vm});
  DISASM_INSN("vmsif.m", vmsif_m, 0, {&vd, &vs2, &opt, &vm});
  DISASM_INSN("viota.m", viota_m, 0, {&vd, &vs2, &opt, &vm});
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
  DISASM_OPIV_VX__INSN(vwmaccsu,  0);
  DISASM_OPIV__X__INSN(vwmaccus,  1);

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

  #define DISASM_VFUNARY0_INSN(name, extra) \
    add_insn(new disasm_insn_t(#name "cvt.xu.f.v", match_##name##cvt_xu_f_v, \
                mask_##name##cvt_xu_f_v, {&vd, &vs2, &opt, &vm})); \
    add_insn(new disasm_insn_t(#name "cvt.x.f.v", match_##name##cvt_x_f_v, \
                mask_##name##cvt_x_f_v, {&vd, &vs2, &opt, &vm})); \
    add_insn(new disasm_insn_t(#name "cvt.f.xu.v", match_##name##cvt_f_xu_v, \
                mask_##name##cvt_f_xu_v, {&vd, &vs2, &opt, &vm})); \
    add_insn(new disasm_insn_t(#name "cvt.f.x.v", match_##name##cvt_f_x_v, \
                mask_##name##cvt_f_x_v, {&vd, &vs2, &opt, &vm}));

  //OPFVV/OPFVF
  //0b01_0000
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
  DISASM_VFUNARY0_INSN(vf,  0);

  DISASM_VFUNARY0_INSN(vfw, 1);
  DISASM_INSN("vfwcvt.f.f.v", vfwcvt_f_f_v, 0, {&vd, &vs2, &opt, &vm});

  DISASM_VFUNARY0_INSN(vfn, 1);
  DISASM_INSN("vfncvt.f.f.v", vfncvt_f_f_v, 0, {&vd, &vs2, &opt, &vm});

  //vfunary1
  DISASM_INSN("vfsqrt.v", vfsqrt_v, 0, {&vd, &vs2, &opt, &vm});
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

  if (xlen == 32) {
    DISASM_INSN("c.flw", c_flw, 0, {&rvc_fp_rs2s, &rvc_lw_address});
    DISASM_INSN("c.flwsp", c_flwsp, 0, {&frd, &rvc_lwsp_address});
    DISASM_INSN("c.fsw", c_fsw, 0, {&rvc_fp_rs2s, &rvc_lw_address});
    DISASM_INSN("c.fswsp", c_fswsp, 0, {&rvc_fp_rs2, &rvc_swsp_address});
    DISASM_INSN("c.jal", c_jal, 0, {&rvc_jump_target});
  } else {
    DISASM_INSN("c.ld", c_ld, 0, {&rvc_rs2s, &rvc_ld_address});
    DISASM_INSN("c.ldsp", c_ldsp, 0, {&xrd, &rvc_ldsp_address});
    DISASM_INSN("c.sd", c_sd, 0, {&rvc_rs2s, &rvc_ld_address});
    DISASM_INSN("c.sdsp", c_sdsp, 0, {&rvc_rs2, &rvc_sdsp_address});
    DISASM_INSN("c.addiw", c_addiw, 0, {&xrd, &rvc_imm});
  }

  // provide a default disassembly for all instructions as a fallback
  #define DECLARE_INSN(code, match, mask) \
   add_insn(new disasm_insn_t(#code " (args unknown)", match, mask, {}));
  #include "encoding.h"
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
