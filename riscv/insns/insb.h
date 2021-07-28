require_extension('P');
reg_t bpos = insn.p_imm3();
require(bpos < (unsigned long)xlen/8); // imm[2] == 1 is illegal on rv32
WRITE_RD(sext_xlen(set_field(RD, make_mask64(bpos * 8, 8), P_B(RS1, 0))));