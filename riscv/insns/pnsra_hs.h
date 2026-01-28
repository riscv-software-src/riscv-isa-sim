require_rv32;
P_NARROW_RD_RS1_LOOP(16, 32, {
    p_rd = p_rs1 >> (P_FIELD(RS2, 0, 16) & 0x1F);
})