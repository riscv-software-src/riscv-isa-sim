require_rv32;
P_NARROW_RD_RS1_ULOOP(8, 16, {
    uint32_t shamt = P_UFIELD(RS2, 0, 8) & 0xF;
    uint32_t result;
    if (shamt == 0) {
        result = p_rs1;
    } else {
        uint32_t shifted  = p_rs1 >> shamt;
        uint32_t roundbit = (p_rs1 >> (shamt - 1)) & 1;
        result = shifted + roundbit;
    }
    p_rd = P_USAT(8, result);
})