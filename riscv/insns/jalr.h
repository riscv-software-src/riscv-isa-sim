reg_t temp = RS1;
WRITE_RD(npc);
set_pc((temp + insn.i_imm()) & ~1);
