require_rv32;
reg_t rd_tmp = RS2;
P_RS1_DW_LOOP(16, {
    rd_tmp += sext_xlen(p_rs1);
})