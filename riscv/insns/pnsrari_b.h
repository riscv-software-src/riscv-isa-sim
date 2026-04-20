require_rv32;
P_NARROW_RD_RS1_LOOP(8, 16, {
    uint32_t shamt = insn.shamth();
    if (shamt != 0) {
        sreg_t val = (sreg_t)p_rs1;
        sreg_t shifted = val >> shamt;
        sreg_t roundbit = (val >> (shamt - 1)) & 1;
        p_rd = (shifted + roundbit) & 0xFF;
    } else {
        p_rd = p_rs1 & 0xFF;
    }
})