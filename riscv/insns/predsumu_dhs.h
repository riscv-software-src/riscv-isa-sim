require_rv32;
reg_t rd_tmp = RS2;
P_RS1_DW_ULOOP(16, {
    rd_tmp += zext_xlen(p_rs1);
})