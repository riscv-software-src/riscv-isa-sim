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
    s << std::hex << "0x" << (uint32_t)insn.rvc_imm();
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
  DEFINE_ITYPE(slli);
  DEFINE_ITYPE(slti);
  add_insn(new disasm_insn_t("seqz", match_sltiu | match_imm_1, mask_sltiu | mask_imm, {&xrd, &xrs1}));
  DEFINE_ITYPE(sltiu);
  add_insn(new disasm_insn_t("not", match_xori | mask_imm, mask_xori | mask_imm, {&xrd, &xrs1}));
  DEFINE_ITYPE(xori);
  DEFINE_ITYPE(srli);
  DEFINE_ITYPE(srai);
  DEFINE_ITYPE(ori);
  DEFINE_ITYPE(andi);
  DEFINE_I1TYPE("sext.w", addiw);
  DEFINE_ITYPE(addiw);
  DEFINE_ITYPE(slliw);
  DEFINE_ITYPE(srliw);
  DEFINE_ITYPE(sraiw);

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
  DEFINE_NOARG(fence);
  DEFINE_NOARG(fence_i);

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
  DEFINE_FXTYPE(fclass_d);
  DEFINE_FXTYPE(fmv_x_d);
  DEFINE_FXTYPE(feq_d);
  DEFINE_FXTYPE(flt_d);
  DEFINE_FXTYPE(fle_d);

  DISASM_INSN("ebreak", c_add, mask_rd | mask_rvc_rs2, {});
  add_insn(new disasm_insn_t("ret", match_c_li | match_rd_ra, mask_c_li | mask_rd | mask_rvc_imm, {}));
  DISASM_INSN("jr", c_li, mask_rvc_imm, {&rvc_rs1});
  DISASM_INSN("jalr", c_lui, mask_rvc_imm, {&rvc_rs1});
  DISASM_INSN("nop", c_addi, mask_rd | mask_rvc_imm, {});
  DISASM_INSN("addi", c_addi16sp, mask_rd, {&rvc_sp, &rvc_sp, &rvc_addi16sp_imm});
  DISASM_INSN("addi", c_addi4spn, 0, {&rvc_rs1s, &rvc_sp, &rvc_addi4spn_imm});
  DISASM_INSN("li", c_li, 0, {&xrd, &rvc_imm});
  DISASM_INSN("lui", c_lui, 0, {&xrd, &rvc_uimm});
  DISASM_INSN("addi", c_addi, 0, {&xrd, &xrd, &rvc_imm});
  DISASM_INSN("slli", c_slli, 0, {&xrd, &rvc_shamt});
  DISASM_INSN("mv", c_mv, 0, {&xrd, &rvc_rs2});
  DISASM_INSN("add", c_add, 0, {&xrd, &xrd, &rvc_rs2});
  DISASM_INSN("addw", c_addw, 0, {&rvc_rs1s, &rvc_rs1s, &rvc_rs2s});
  DISASM_INSN("sub", c_sub, 0, {&rvc_rs1s, &rvc_rs1s, &rvc_rs2s});
  DISASM_INSN("subw", c_subw, 0, {&rvc_rs1s, &rvc_rs1s, &rvc_rs2s});
  DISASM_INSN("and", c_and, 0, {&rvc_rs1s, &rvc_rs1s, &rvc_rs2s});
  DISASM_INSN("or", c_or, 0, {&rvc_rs1s, &rvc_rs1s, &rvc_rs2s});
  DISASM_INSN("xor", c_xor, 0, {&rvc_rs1s, &rvc_rs1s, &rvc_rs2s});
  DISASM_INSN("lw", c_lwsp, 0, {&xrd, &rvc_lwsp_address});
  DISASM_INSN("fld", c_fld, 0, {&rvc_rs2s, &rvc_ld_address});
  DISASM_INSN("sw", c_swsp, 0, {&rvc_rs2, &rvc_swsp_address});
  DISASM_INSN("lw", c_lw, 0, {&rvc_rs2s, &rvc_lw_address});
  DISASM_INSN("sw", c_sw, 0, {&rvc_rs2s, &rvc_lw_address});
  DISASM_INSN("beqz", c_beqz, 0, {&rvc_rs1s, &rvc_branch_target});
  DISASM_INSN("bnez", c_bnez, 0, {&rvc_rs1s, &rvc_branch_target});
  DISASM_INSN("j", c_j, 0, {&rvc_jump_target});

  if (xlen == 32) {
    DISASM_INSN("flw", c_flw, 0, {&rvc_rs2s, &rvc_lw_address});
    DISASM_INSN("flw", c_flwsp, 0, {&xrd, &rvc_lwsp_address});
    DISASM_INSN("fsw", c_fsw, 0, {&rvc_rs2s, &rvc_lw_address});
    DISASM_INSN("fsw", c_fswsp, 0, {&rvc_rs2, &rvc_swsp_address});
    DISASM_INSN("jal", c_jal, 0, {&rvc_jump_target});
  } else {
    DISASM_INSN("ld", c_ld, 0, {&rvc_rs2s, &rvc_ld_address});
    DISASM_INSN("ld", c_ldsp, 0, {&xrd, &rvc_ldsp_address});
    DISASM_INSN("sd", c_sd, 0, {&rvc_rs2s, &rvc_ld_address});
    DISASM_INSN("sd", c_sdsp, 0, {&rvc_rs2, &rvc_sdsp_address});
    DISASM_INSN("addiw", c_addiw, 0, {&xrd, &xrd, &rvc_imm});
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
