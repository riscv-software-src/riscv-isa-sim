require_rv32;
P_WIDEN_REDUCTION_ULOOP(32, 16, true, false, {
  p_res += (uint32_t)p_rs1 * (uint32_t)p_rs2;
})