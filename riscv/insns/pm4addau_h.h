require_rv64;
P_REDUCTION_ULOOP(64, 16, true, false, {
  p_res += (reg_t)p_rs1 * p_rs2;
})