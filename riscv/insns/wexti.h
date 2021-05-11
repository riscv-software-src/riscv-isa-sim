require_extension('P');
reg_t rs1 = (xlen == 32) ? RS1_PAIR : RS1;
reg_t lsb = insn.p_imm5();
WRITE_RD(sext32(get_field(rs1, make_mask64(lsb, 32))));