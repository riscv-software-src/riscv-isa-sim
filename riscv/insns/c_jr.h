require_extension(EXT_ZCA);
require(insn.rvc_rs1() != 0);
set_pc(RVC_RS1 & ~reg_t(1));

if (ZICFILP_xLPE(STATE.v, STATE.prv)) {
  STATE.elp = ZICFILP_IS_LP_EXPECTED(insn.rvc_rs1());
  serialize();
}
