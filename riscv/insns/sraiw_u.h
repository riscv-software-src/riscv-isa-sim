require_rv64;
require_extension('P');

reg_t sa = insn.p_imm5();
if (sa != 0) {
  WRITE_RD(sext32(((P_SW(RS1, 0) >> (sa - 1)) + 1) >> 1));
} else {
  WRITE_RD(sext32(P_SW(RS1, 0)));
}