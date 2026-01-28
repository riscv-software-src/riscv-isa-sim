require_rv32;
P_NARROW_RD_RS1_ULOOP(16, 32, {
    p_rd = P_USAT_FULL(16, (sreg_t)(p_rs1 >> (P_UFIELD(RS2, 0, 16) & 0X1F)));
})