reg_t rd_tmp = RS2; \
P_RS1_LOOP_BASE(16)
  P_RS1_PARAMS(16)
  rd_tmp += sext_xlen(p_rs1);
P_RD_LOOP_END()
