P_REDUCTION_LOOP(32, 16, false, false, {
  p_res += ((p_rs1 * p_rs2) + 0x4000) >> 15;
})