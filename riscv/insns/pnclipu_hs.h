require_rv32;
P_NARROW_RD_RS1_ULOOP(16, 32, {
    p_rd = P_USAT(16, p_rs1 >> (P_UFIELD(RS2, 0, 16) & 0X1F));
})