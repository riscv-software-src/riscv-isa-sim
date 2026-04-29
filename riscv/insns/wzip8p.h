require_rv32;
P_WIDEN_RD_RS1_RS2_ZIP_LOOP(8, 8, {
    p_rd = p_rs2 << 8 | p_rs1;
})