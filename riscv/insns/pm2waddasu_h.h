require_rv32;
P_WIDEN_REDUCTION_SULOOP(32, 16, true, false, {
  p_res += p_rs1 * p_rs2;
})