require_extension('P');
P_RD_RS1_RS2_ZIP_LOOP(8, 8, 8, 1, {
    p_rd = i % 2 ? p_rs2 : p_rs1;
})
