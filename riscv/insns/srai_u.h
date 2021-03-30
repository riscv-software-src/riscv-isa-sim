require_extension('P');
sreg_t rs1 = sext_xlen(RS1);
reg_t sa = xlen == 32 ? insn.p_imm5() : insn.p_imm6();

if (sa > 0) {
  WRITE_RD(sext_xlen(((rs1 >> (sa - 1)) + 1) >> 1));
} else {
  WRITE_RD(sext_xlen(rs1));
}