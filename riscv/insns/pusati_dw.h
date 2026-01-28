require_rv32;
P_RD_RS1_DW_ULOOP(32, 32, {
  uint64_t uint_max = insn.shamtw() ? UINT64_MAX >> (64 - insn.shamtw()) : 0;
  int32_t s = (int32_t)p_rs1;
  p_rd = p_rs1;
  if (s < 0) {
    p_rd = 0;
  } else if (s > uint_max) {
    p_rd = uint_max;
  }
})