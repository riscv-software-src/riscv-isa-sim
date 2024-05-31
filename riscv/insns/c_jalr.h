require_extension(EXT_ZCA);
require(insn.rvc_rs1() != 0);
reg_t tmp = npc;
set_pc(RVC_RS1 & ~reg_t(1));
WRITE_REG(X_RA, tmp);

if (ZICFILP_xLPE(STATE.v, STATE.prv)) {
  STATE.elp = ZICFILP_IS_LP_EXPECTED(insn.rvc_rs1());
  serialize();
}
