require_extension('P');
reg_t bpos = (xlen == 32) ? insn.p_imm2() : insn.p_imm3();
WRITE_RD(set_field(RD, make_mask64(bpos * 8, 8), P_B(RS1, 0)));