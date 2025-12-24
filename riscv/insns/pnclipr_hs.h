require_rv32;
P_NARROW_RD_RS1_LOOP(16, 32, {
    uint32_t shamt = P_FIELD(RS2, 0, 16) & 0x1F;
    sreg_t result;
    if (shamt == 0) {
        result = p_rs1;
    } else {
        sreg_t shifted = (sreg_t)p_rs1 >> shamt;
        sreg_t roundbit = ((sreg_t)p_rs1 >> (shamt - 1)) & 1;
        result = shifted + roundbit;
    }
    p_rd = P_SAT(16, result);
})