require_rv64;
reg_t rd_tmp = RS2; \
P_RS1_LOOP_BASE(32)
  P_RS1_PARAMS(32)
  rd_tmp += sext_xlen(p_rs1);
P_RD_LOOP_END()
