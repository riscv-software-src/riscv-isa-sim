require_rv32;
P_NARROW_RD_RS1_ULOOP(8, 16, {
    p_rd = P_USAT(8, p_rs1 >> (P_UFIELD(RS2, 0, 8) & 0xF));
})