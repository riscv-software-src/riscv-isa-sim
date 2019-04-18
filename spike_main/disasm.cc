// See LICENSE for license details.

#include "disasm.h"
#include <string>
#include <vector>
#include <tuple>
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

  DEFINE_FLOAD(flh)
  DEFINE_FLOAD(flw)
  DEFINE_FLOAD(fld)
  DEFINE_FLOAD(flq)

  DEFINE_FSTORE(fsh)
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
  DISASM_INSN("c.fldsp", c_fldsp, 0, {&rvc_fp_rs2s, &rvc_ldsp_address});
  DISASM_INSN("c.fsd", c_fsd, 0, {&rvc_fp_rs2s, &rvc_ld_address});
  DISASM_INSN("c.fsdsp", c_fsdsp, 0, {&rvc_fp_rs2s, &rvc_sdsp_address});

  DISASM_INSN("vsetvli", vsetvli, 0, {&xrd, &xrs1, &v_vtype});
  DISASM_INSN("vsetvl", vsetvl, 0, {&xrd, &xrs1, &xrs2});

  #define DEFINE_VMEM_LD_TYPE(name, fmt) \
    add_insn(new disasm_insn_t("vl" #name "b.v",  match_vl##name##b_v,  mask_vl##name##b_v, fmt)); \
    add_insn(new disasm_insn_t("vl" #name "h.v",  match_vl##name##h_v,  mask_vl##name##h_v, fmt)); \
    add_insn(new disasm_insn_t("vl" #name "w.v",  match_vl##name##w_v,  mask_vl##name##w_v, fmt)); \
    add_insn(new disasm_insn_t("vl" #name "e.v",  match_vl##name##e_v,  mask_vl##name##e_v, fmt)); \
    add_insn(new disasm_insn_t("vl" #name "bu.v", match_vl##name##bu_v, mask_vl##name##bu_v, fmt)); \
    add_insn(new disasm_insn_t("vl" #name "hu.v", match_vl##name##hu_v, mask_vl##name##hu_v, fmt)); \
    add_insn(new disasm_insn_t("vl" #name "wu.v", match_vl##name##wu_v, mask_vl##name##wu_v, fmt));

  #define DEFINE_VMEM_ST_TYPE(name, fmt) \
    add_insn(new disasm_insn_t("vs" #name "b.v", match_vs##name##b_v, mask_vs##name##b_v, fmt)); \
    add_insn(new disasm_insn_t("vs" #name "h.v", match_vs##name##h_v, mask_vs##name##h_v, fmt)); \
    add_insn(new disasm_insn_t("vs" #name "w.v", match_vs##name##w_v, mask_vs##name##w_v, fmt)); \
    add_insn(new disasm_insn_t("vs" #name "e.v", match_vs##name##e_v, mask_vs##name##e_v, fmt));

  const std::vector<const arg_t *> v_ld_unit = {&vd, &v_address, &opt, &vm};
  const std::vector<const arg_t *> v_st_unit = {&vs3, &v_address, &opt, &vm};
  const std::vector<const arg_t *> v_ld_stride = {&vd, &v_address, &xrs2, &opt, &vm};
  const std::vector<const arg_t *> v_st_stride = {&vs3, &v_address, &xrs2, &opt, &vm};
  const std::vector<const arg_t *> v_ld_index = {&vd, &v_address, &vs2, &opt, &vm};
  const std::vector<const arg_t *> v_st_index = {&vs3, &v_address, &vs2, &opt, &vm};

  DEFINE_VMEM_LD_TYPE(, v_ld_unit);
  DEFINE_VMEM_ST_TYPE(, v_st_unit);
  DEFINE_VMEM_LD_TYPE(s, v_ld_stride);
  DEFINE_VMEM_ST_TYPE(s, v_st_stride);
  DEFINE_VMEM_LD_TYPE(x, v_ld_index);
  DEFINE_VMEM_ST_TYPE(x, v_st_index);

  #undef DEFINE_VMEM_LD_TYPE
  #undef DEFINE_VMEM_ST_TYPE

  // handle vector segment load/store
  for (size_t nf = 1; nf <= 7; ++nf) {
    std::pair<reg_t, reg_t> insn_code[] = {
      {mask_vlb_v,  mask_vlb_v},
      {mask_vlh_v,  mask_vlh_v},
      {mask_vlw_v,  mask_vlw_v},
      {mask_vle_v,  mask_vle_v},
      {mask_vlbu_v, mask_vlbu_v},
      {mask_vlhu_v, mask_vlhu_v},
      {mask_vlwu_v, mask_vlwu_v},
      {mask_vsb_v,  mask_vsb_v},
      {mask_vsh_v,  mask_vsh_v},
      {mask_vsw_v,  mask_vsw_v},
      {mask_vse_v,  mask_vse_v},

      {mask_vlsb_v,  mask_vlsb_v},
      {mask_vlsh_v,  mask_vlsh_v},
      {mask_vlsw_v,  mask_vlsw_v},
      {mask_vlse_v,  mask_vlse_v},
      {mask_vlsbu_v, mask_vlsbu_v},
      {mask_vlshu_v, mask_vlshu_v},
      {mask_vlswu_v, mask_vlswu_v},
      {mask_vssb_v,  mask_vssb_v},
      {mask_vssh_v,  mask_vssh_v},
      {mask_vssw_v,  mask_vssw_v},
      {mask_vsse_v,  mask_vssw_v},

      {mask_vlxb_v,  mask_vlxb_v},
      {mask_vlxh_v,  mask_vlxh_v},
      {mask_vlxw_v,  mask_vlxw_v},
      {mask_vlxe_v,  mask_vlxe_v},
      {mask_vlxbu_v, mask_vlxbu_v},
      {mask_vlxhu_v, mask_vlxhu_v},
      {mask_vlxwu_v, mask_vlxwu_v},
      {mask_vsxb_v,  mask_vsxb_v},
      {mask_vsxh_v,  mask_vsxh_v},
      {mask_vsxw_v,  mask_vsxw_v},
      {mask_vsxe_v,  mask_vsxw_v},

    };

    std::pair<const char *, std::vector<const arg_t*>> fmts[] = { 
      {"vlseg%db_v", {&vd, &v_address, &opt, &vm}},
      {"vlseg%dh_v", {&vd, &v_address, &opt, &vm}},
      {"vlseg%dw_v", {&vd, &v_address, &opt, &vm}},
      {"vlseg%de_v", {&vd, &v_address, &opt, &vm}},
      {"vlseg%dwu_v", {&vd, &v_address, &opt, &vm}},
      {"vlseg%dhu_v", {&vd, &v_address, &opt, &vm}},
      {"vlseg%dbu_v", {&vd, &v_address, &opt, &vm}},
      {"vsseg%db_v", {&vs3, &v_address, &opt, &vm}},
      {"vsseg%dh_v", {&vs3, &v_address, &opt, &vm}},
      {"vsseg%dw_v", {&vs3, &v_address, &opt, &vm}},
      {"vsseg%de_v", {&vs3, &v_address, &opt, &vm}},

      {"vlsseg%db_v", {&vd, &v_address, &xrs2, &opt, &vm}},
      {"vlsseg%dh_v", {&vd, &v_address, &xrs2, &opt, &vm}},
      {"vlsseg%dw_v", {&vd, &v_address, &xrs2, &opt, &vm}},
      {"vlsseg%de_v", {&vd, &v_address, &xrs2, &opt, &vm}},
      {"vlsseg%dbu_v",{&vd, &v_address, &xrs2, &opt, &vm}},
      {"vlsseg%dhu_v",{&vd, &v_address, &xrs2, &opt, &vm}},
      {"vlsseg%dwu_v",{&vd, &v_address, &xrs2, &opt, &vm}},
      {"vssseg%db_v", {&vs3, &v_address, &xrs2, &opt, &vm}},
      {"vssseg%dh_v", {&vs3, &v_address, &xrs2, &opt, &vm}},
      {"vssseg%dw_v", {&vs3, &v_address, &xrs2, &opt, &vm}},
      {"vssseg%de_v", {&vs3, &v_address, &xrs2, &opt, &vm}},

      {"vlseg%db_v", {&vd, &v_address, &vs2, &opt, &vm}},
      {"vlseg%dh_v", {&vd, &v_address, &vs2, &opt, &vm}},
      {"vlseg%dw_v", {&vd, &v_address, &vs2, &opt, &vm}},
      {"vlseg%de_v", {&vd, &v_address, &vs2, &opt, &vm}},
      {"vlseg%dwu_v",{&vd, &v_address, &vs2, &opt, &vm}},
      {"vlseg%dhu_v",{&vd, &v_address, &vs2, &opt, &vm}},
      {"vlseg%dbu_v",{&vd, &v_address, &vs2, &opt, &vm}},
      {"vsseg%db_v", {&vs3, &v_address, &vs2, &opt, &vm}},
      {"vsseg%dh_v", {&vs3, &v_address, &vs2, &opt, &vm}},
      {"vsseg%dw_v", {&vs3, &v_address, &vs2, &opt, &vm}},
      {"vsseg%de_v", {&vs3, &v_address, &vs2, &opt, &vm}},
    };

    for (size_t idx_insn = 0; idx_insn < sizeof(insn_code) / sizeof(insn_code[0]); ++idx_insn) {
      const reg_t code_nf = nf << 29;
      char buf[128];
      sprintf(buf, fmts[idx_insn].first,
                   insn_code[idx_insn].first | code_nf,
                   insn_code[idx_insn].second);
      add_insn(new disasm_insn_t(buf,
                                 insn_code[idx_insn].first | nf,
                                 insn_code[idx_insn].second,
                                 fmts[idx_insn].second
                                 ));
    }
  }


  #define DEFINE_OPIV_VXI_TYPE(name, sign) \
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

  #define DEFINE_OPIV_VX__TYPE(name, sign) \
    add_insn(new disasm_insn_t(#name ".vv", match_##name##_vv, mask_##name##_vv, \
                {&vd, &vs2, &vs1, &opt, &vm})); \
    add_insn(new disasm_insn_t(#name ".vx", match_##name##_vx, mask_##name##_vx, \
                {&vd, &vs2, &xrs1, &opt, &vm})); \

  #define DEFINE_OPIV__XI_TYPE(name, sign) \
    add_insn(new disasm_insn_t(#name ".vx", match_##name##_vx, mask_##name##_vx, \
                {&vd, &vs2, &xrs1, &opt, &vm})); \
    if (sign) \
    add_insn(new disasm_insn_t(#name ".vi", match_##name##_vi, mask_##name##_vi, \
                {&vd, &vs2, &v_simm5, &opt, &vm})); \
    else \
    add_insn(new disasm_insn_t(#name ".vi", match_##name##_vi, mask_##name##_vi, \
                {&vd, &vs2, &zimm5, &opt, &vm}));

  #define DEFINE_OPIV_V___TYPE(name, sign) \
    add_insn(new disasm_insn_t(#name ".vv", match_##name##_vv, mask_##name##_vv, \
                {&vd, &vs2, &vs1, &opt, &vm}));

  #define DEFINE_OPIV_S___TYPE(name, sign) \
    add_insn(new disasm_insn_t(#name ".vs", match_##name##_vs, mask_##name##_vs, \
                {&vd, &vs2, &vs1, &opt, &vm}));

  #define DEFINE_OPIV_W___TYPE(name, sign) \
    add_insn(new disasm_insn_t(#name ".wv", match_##name##_wv, mask_##name##_wv, \
                {&vd, &vs2, &vs1, &opt, &vm}));

  #define DEFINE_OPIV_M___TYPE(name, sign) \
    add_insn(new disasm_insn_t(#name ".mm", match_##name##_mm, mask_##name##_mm, \
                {&vd, &vs2, &vs1}));

  #define DEFINE_OPIV__X__TYPE(name, sign) \
    add_insn(new disasm_insn_t(#name ".vx", match_##name##_vx, mask_##name##_vx, \
                {&vd, &vs2, &xrs1, &opt, &vm}));

  //OPFVV/OPFVF
  //0b00_0000
  DEFINE_OPIV_VXI_TYPE(vadd,      1);
  DEFINE_OPIV_VX__TYPE(vsub,      1);
  DEFINE_OPIV__XI_TYPE(vrsub,     1);
  DEFINE_OPIV_VX__TYPE(vminu,     0);
  DEFINE_OPIV_VX__TYPE(vmin,      1);
  DEFINE_OPIV_VX__TYPE(vmaxu,     1);
  DEFINE_OPIV_VX__TYPE(vmax,      0);
  DEFINE_OPIV_VXI_TYPE(vand,      1);
  DEFINE_OPIV_VXI_TYPE(vor,       1);
  DEFINE_OPIV_VXI_TYPE(vxor,      1);
  DEFINE_OPIV_VXI_TYPE(vrgather,  1);
  DEFINE_OPIV__XI_TYPE(vslideup,  1);
  DEFINE_OPIV__XI_TYPE(vslidedown,1);

  //0b01_0000
  DEFINE_OPIV_VXI_TYPE(vadc,      1);
  DEFINE_OPIV_VX__TYPE(vsbc,      1);
  DEFINE_OPIV_VXI_TYPE(vmerge,    1);
  DEFINE_OPIV_VXI_TYPE(vseq,      1);
  DEFINE_OPIV_VXI_TYPE(vsne,      1);
  DEFINE_OPIV_VX__TYPE(vsltu,     0);
  DEFINE_OPIV_VX__TYPE(vslt,      1);
  DEFINE_OPIV_VXI_TYPE(vsleu,     0);
  DEFINE_OPIV_VXI_TYPE(vsle,      1);
  DEFINE_OPIV__XI_TYPE(vsgtu,     0);
  DEFINE_OPIV__XI_TYPE(vsgt,      1);

  //0b10_0000
  DEFINE_OPIV_VXI_TYPE(vsaddu,    0);
  DEFINE_OPIV_VXI_TYPE(vsadd,     1);
  DEFINE_OPIV_VX__TYPE(vssubu,    0);
  DEFINE_OPIV_VX__TYPE(vssub,     1);
  DEFINE_OPIV_VXI_TYPE(vaadd,     1);
  DEFINE_OPIV_VXI_TYPE(vsll,      1);
  DEFINE_OPIV_VX__TYPE(vasub,     1);
  DEFINE_OPIV_VX__TYPE(vsmul,     1);
  DEFINE_OPIV_VXI_TYPE(vsrl,      1);
  DEFINE_OPIV_VXI_TYPE(vsra,      1);
  DEFINE_OPIV_VXI_TYPE(vssrl,     1);
  DEFINE_OPIV_VXI_TYPE(vssra,     1);
  DEFINE_OPIV_VXI_TYPE(vnsrl,     1);
  DEFINE_OPIV_VXI_TYPE(vnsra,     1);
  DEFINE_OPIV_VXI_TYPE(vnclipu,   0);
  DEFINE_OPIV_VXI_TYPE(vnclip,    1);

  //0b11_0000
  DEFINE_OPIV_S___TYPE(vwredsumu, 0);
  DEFINE_OPIV_S___TYPE(vwredsum,  1);
  DEFINE_OPIV_V___TYPE(vdotu,     0);
  DEFINE_OPIV_V___TYPE(vdot,      1);
  DEFINE_OPIV_VX__TYPE(vwsmaccu,  0);
  DEFINE_OPIV_VX__TYPE(vwsmacc,   1);
  DEFINE_OPIV_VX__TYPE(vwsmsacu,  0);
  DEFINE_OPIV_VX__TYPE(vwsmsac,   1);

  //OPMVV/OPMVX
  //0b00_0000
  DEFINE_OPIV_V___TYPE(vredsum,   1);
  DEFINE_OPIV_V___TYPE(vredand,   1);
  DEFINE_OPIV_V___TYPE(vredor,    1);
  DEFINE_OPIV_V___TYPE(vredxor,   1);
  DEFINE_OPIV_V___TYPE(vredminu,  0);
  DEFINE_OPIV_V___TYPE(vredmin,   1);
  DEFINE_OPIV_V___TYPE(vredmaxu,  0);
  DEFINE_OPIV_V___TYPE(vredmax,   1);

  add_insn(new disasm_insn_t("vext.x.v", match_vext_x_v, mask_vext_x_v,
              {&xrd, &vs2, &xrs1}));
  add_insn(new disasm_insn_t("vmv.s.x", match_vmv_s_x, mask_vmv_s_x,
              {&vd, &xrs1}));
  DEFINE_OPIV__X__TYPE(vslide1up,  1);
  DEFINE_OPIV__X__TYPE(vslide1down,1);

  //0b01_0000
  add_insn(new disasm_insn_t("vmpopc.m", match_vmpopc_m, mask_vmpopc_m,
              {&xrd, &vs2, &opt, &vm}));
  add_insn(new disasm_insn_t("vmfirst.m", match_vmfirst_m, mask_vmfirst_m,
              {&xrd, &vs2, &opt, &vm}));
  add_insn(new disasm_insn_t("vcompress.vm", match_vcompress_vm, mask_vcompress_vm,
              {&vd, &vs2, &opt, &vm}));

  DEFINE_OPIV_M___TYPE(vmandnot,  1);
  DEFINE_OPIV_M___TYPE(vmand,     1);
  DEFINE_OPIV_M___TYPE(vmor,      1);
  DEFINE_OPIV_M___TYPE(vmxor,     1);
  DEFINE_OPIV_M___TYPE(vmornot,   1);
  DEFINE_OPIV_M___TYPE(vmnand,    1);
  DEFINE_OPIV_M___TYPE(vmnor,     1);
  DEFINE_OPIV_M___TYPE(vmxnor,    1);

  //0b10_0000
  DEFINE_OPIV_VX__TYPE(vdivu,     0);
  DEFINE_OPIV_VX__TYPE(vdiv,      1);
  DEFINE_OPIV_VX__TYPE(vremu,     0);
  DEFINE_OPIV_VX__TYPE(vrem,      1);
  DEFINE_OPIV_VX__TYPE(vmulhu,    0);
  DEFINE_OPIV_VX__TYPE(vmul,      1);
  DEFINE_OPIV_VX__TYPE(vmulhsu,   0);
  DEFINE_OPIV_VX__TYPE(vmulh,     1);
  DEFINE_OPIV_VX__TYPE(vmadd,     1);
  DEFINE_OPIV_VX__TYPE(vmsub,     1);
  DEFINE_OPIV_VX__TYPE(vmacc,     1);
  DEFINE_OPIV_VX__TYPE(vmsac,     1);

  //0b11_0000
  DEFINE_OPIV_VX__TYPE(vwaddu,    0);
  DEFINE_OPIV_VX__TYPE(vwadd,     1);
  DEFINE_OPIV_VX__TYPE(vwsubu,    0);
  DEFINE_OPIV_VX__TYPE(vwsub,     1);
  DEFINE_OPIV_W___TYPE(vwaddu,    0);
  DEFINE_OPIV_W___TYPE(vwadd,     1);
  DEFINE_OPIV_W___TYPE(vwsubu,    0);
  DEFINE_OPIV_W___TYPE(vwsub,     1);
  DEFINE_OPIV_VX__TYPE(vwmulu,    0);
  DEFINE_OPIV_VX__TYPE(vwmulsu,   0);
  DEFINE_OPIV_VX__TYPE(vwmul,     1);
  DEFINE_OPIV_VX__TYPE(vwmaccu,   0);
  DEFINE_OPIV_VX__TYPE(vwmacc,    1);
  DEFINE_OPIV_VX__TYPE(vwmsacu,   0);
  DEFINE_OPIV_VX__TYPE(vwmsac,    1);

  #undef DEFINE_OPIV_VXI_TYPE
  #undef DEFINE_OPIV_VX__TYPE
  #undef DEFINE_OPIV__XI_TYPE
  #undef DEFINE_OPIV_V___TYPE
  #undef DEFINE_OPIV_S___TYPE
  #undef DEFINE_OPIV_W___TYPE
  #undef DEFINE_OPIV_M___TYPE
  #undef DEFINE_OPIV__X__TYPE

  #define DEFINE_OPIV_VF_TYPE(name) \
      add_insn(new disasm_insn_t(#name ".vv", match_##name##_vv, mask_##name##_vv, \
                  {&vd, &vs2, &vs1, &opt, &vm})); \
      add_insn(new disasm_insn_t(#name ".vf", match_##name##_vf, mask_##name##_vf, \
                  {&vd, &vs2, &xrs1, &opt, &vm})); \

  #define DEFINE_OPIV_WF_TYPE(name) \
      add_insn(new disasm_insn_t(#name ".wv", match_##name##_wv, mask_##name##_wv, \
                  {&vd, &vs2, &vs1, &opt, &vm})); \
      add_insn(new disasm_insn_t(#name ".wf", match_##name##_wf, mask_##name##_wf, \
                  {&vd, &vs2, &xrs1, &opt, &vm})); \

  #define DEFINE_OPIV_V__TYPE(name) \
      add_insn(new disasm_insn_t(#name ".vv", match_##name##_vv, mask_##name##_vv, \
                  {&vd, &vs2, &vs1, &opt, &vm}));

  #define DEFINE_OPIV_S__TYPE(name) \
      add_insn(new disasm_insn_t(#name ".vs", match_##name##_vs, mask_##name##_vs, \
                  {&vd, &vs2, &vs1, &opt, &vm}));

  #define DEFINE_OPIV__F_TYPE(name) \
    add_insn(new disasm_insn_t(#name ".vf", match_##name##_vf, mask_##name##_vf, \
                {&vd, &vs2, &xrs1, &opt, &vm})); \

  //OPFVV/OPFVF
  //0b01_0000
  DEFINE_OPIV_VF_TYPE(vfadd);
  DEFINE_OPIV_S__TYPE(vfredsum);
  DEFINE_OPIV_VF_TYPE(vfsub);
  DEFINE_OPIV_S__TYPE(vfredosum);
  DEFINE_OPIV_VF_TYPE(vfmin);
  DEFINE_OPIV_S__TYPE(vfredmin);
  DEFINE_OPIV_VF_TYPE(vfmax);
  DEFINE_OPIV_S__TYPE(vfredmax);
  DEFINE_OPIV_VF_TYPE(vfsgnj);
  DEFINE_OPIV_VF_TYPE(vfsgnn);
  DEFINE_OPIV_VF_TYPE(vfsgnx);
  add_insn(new disasm_insn_t("vfmv.f.s", match_vfmv_f_s, mask_vfmv_f_s,
              {&vd, &vs2}));
  add_insn(new disasm_insn_t("vfmv.s.f", match_vfmv_s_f, mask_vfmv_s_f,
              {&vd, &xrs1}));

  //0b01_0000
  DEFINE_OPIV__F_TYPE(vfmerge);
  DEFINE_OPIV_VF_TYPE(vfeq);
  DEFINE_OPIV_VF_TYPE(vfle);
  DEFINE_OPIV_VF_TYPE(vford);
  DEFINE_OPIV_VF_TYPE(vflt);
  DEFINE_OPIV_VF_TYPE(vfne);
  DEFINE_OPIV__F_TYPE(vfgt);
  DEFINE_OPIV__F_TYPE(vfge);

  //0b10_0000
  DEFINE_OPIV_VF_TYPE(vfdiv);
  DEFINE_OPIV__F_TYPE(vfrdiv);
  DEFINE_OPIV_V__TYPE(vfunary0);
  DEFINE_OPIV_V__TYPE(vfunary1);
  DEFINE_OPIV_VF_TYPE(vfmul);
  DEFINE_OPIV_VF_TYPE(vfmadd);
  DEFINE_OPIV_VF_TYPE(vfnmadd);
  DEFINE_OPIV_VF_TYPE(vfmsub);
  DEFINE_OPIV_VF_TYPE(vfnmsub);
  DEFINE_OPIV_VF_TYPE(vfmacc);
  DEFINE_OPIV_VF_TYPE(vfnmacc);
  DEFINE_OPIV_VF_TYPE(vfmsac);
  DEFINE_OPIV_VF_TYPE(vfnmsac);

  //0b11_0000
  DEFINE_OPIV_VF_TYPE(vfwadd);
  DEFINE_OPIV_S__TYPE(vfwredsum);
  DEFINE_OPIV_VF_TYPE(vfwsub);
  DEFINE_OPIV_S__TYPE(vfwredosum);
  DEFINE_OPIV_WF_TYPE(vfwadd);
  DEFINE_OPIV_WF_TYPE(vfwsub);
  DEFINE_OPIV_VF_TYPE(vfwmul);
  DEFINE_OPIV_V__TYPE(vfdot);
  DEFINE_OPIV_VF_TYPE(vfwmacc);
  DEFINE_OPIV_VF_TYPE(vfwnmacc);
  DEFINE_OPIV_VF_TYPE(vfwmsac);
  DEFINE_OPIV_VF_TYPE(vfwnmsac);

  #undef DEFINE_OPIV_VF_TYPE
  #undef DEFINE_OPIV_V__TYPE
  #undef DEFINE_OPIV__F_TYPE
  #undef DEFINE_OPIV_S__TYPE
  #undef DEFINE_OPIV_W__TYPE

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

void disassembler_t::add_insn(disasm_insn_t* insn)
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
