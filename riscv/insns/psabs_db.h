require_rv32;
P_RD_RS1_DW_LOOP(8, 8, {
  sreg_t abs_val = p_rs1 > 0 ? p_rs1 : -sext32(p_rs1);
  p_rd = P_SAT(8, abs_val);
  if (p_rd != abs_val) P.VU.vxsat->write(1);
})