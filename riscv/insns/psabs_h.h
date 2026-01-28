P_RD_RS1_LOOP(16, 16, {
  sreg_t abs_val = p_rs1 > 0 ? p_rs1 : -sext32(p_rs1);
  p_rd = P_SAT(16, abs_val);
  if (p_rd != abs_val) P.VU.vxsat->write(1);
})