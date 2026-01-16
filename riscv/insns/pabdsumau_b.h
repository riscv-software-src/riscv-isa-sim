P_REDUCTION_ULOOP(64, 8, true, false, {
  p_res += (p_rs1 > p_rs2 ? p_rs1 - p_rs2 : p_rs2 - p_rs1);
})
