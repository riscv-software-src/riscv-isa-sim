CHECK_RD();
reg_t tmp = npc;
set_pc((RS1 + insn.i_imm()) & ~reg_t(1));
WRITE_RD(tmp);

if (ZICFILP_xLPE(STATE.v, STATE.prv)) {
  STATE.elp = ZICFILP_IS_LP_EXPECTED(insn.rs1());
  serialize();
}
