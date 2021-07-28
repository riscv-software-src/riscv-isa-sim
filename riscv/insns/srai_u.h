require_extension('P');
sreg_t rs1 = sext_xlen(RS1);
reg_t sa = insn.p_imm6();
require(sa < (unsigned long)xlen); // imm[5] == 1 is illegal on rv32

if (sa > 0) {
  WRITE_RD(sext_xlen(((rs1 >> (sa - 1)) + 1) >> 1));
} else {
  WRITE_RD(sext_xlen(rs1));
}