require_rv32;
P_NARROW_RD_RS1_ULOOP(8, 16, {
    uint8_t shamt = (uint8_t)(P_UFIELD(RS2, 0, 8) & 0xF);
    p_rd = (uint8_t)((uint16_t)p_rs1 >> shamt);
})
