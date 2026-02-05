require_extension('P');
require_rv64;
P_RD_RS1_RS2_ZIP_LOOP(16, 16, 16, 0, {
    p_rd = i % 2 ? p_rs2 : p_rs1;
})
