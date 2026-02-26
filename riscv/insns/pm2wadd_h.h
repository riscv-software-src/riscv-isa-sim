require_rv32;
P_WIDEN_REDUCTION_LOOP(32, 16, false, false, {
  p_res += sext32(p_rs1) * sext32(p_rs2);
})