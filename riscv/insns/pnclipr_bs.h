require_rv32;
P_NARROW_RD_RS1_LOOP(8, 16, {
    uint16_t shamt = P_FIELD(RS2, 0, 8) & 0xF;
    sreg_t result;
    if (shamt == 0) {
        result = p_rs1;
    } else {
        sreg_t shifted = (sreg_t)p_rs1 >> shamt;
        sreg_t roundbit = ((sreg_t)p_rs1 >> (shamt - 1)) & 1;
        result = shifted + roundbit;
    }
    p_rd = P_SAT(8, result);
})