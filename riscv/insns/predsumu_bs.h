reg_t rd_tmp = RS2; \
P_RS1_LOOP_BASE(8)
  P_RS1_UPARAMS(8)
  rd_tmp += zext_xlen(p_rs1);
P_RD_LOOP_END()