require_rv32;
P_WIDEN_RD_RS1_RS2_ZIP_LOOP(16, 16, {
    p_rd = p_rs2 << 16 | p_rs1;
})