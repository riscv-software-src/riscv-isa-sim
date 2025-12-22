require_rv64;
P_REDUCTION_LOOP(64, 32, true, false, {
  p_res += (((sreg_t)p_rs1 * p_rs2) + 0x40000000) >> 31;
})