require_rv32;
P_RD_RS1_DW_ULOOP(16, 16, {
  uint64_t uint_max = insn.shamth() ? UINT64_MAX >> (64 - insn.shamth()) : 0;
  int16_t s = (int16_t)p_rs1;
  p_rd = p_rs1;
  if (s < 0) {
    p_rd = 0;
  } else if (s > uint_max) {
    p_rd = uint_max;
  }
})