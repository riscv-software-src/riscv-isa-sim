require_rv32;
P_RD_RS1_RS2_DW_ULOOP(32,32,32, {
  p_rd = ((reg_t)p_rs1 + (reg_t)p_rs2) >> 1;
})