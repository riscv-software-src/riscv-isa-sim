require_rv32;
P_NARROW_RD_RS1_ULOOP(16, 32, {
    uint16_t shamt = (uint16_t)(P_UFIELD(RS2, 0, 16) & 0x1F);
    p_rd = (uint16_t)((uint32_t)p_rs1 >> shamt);
})