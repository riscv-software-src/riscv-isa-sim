require_rv32;
P_NARROW_RD_RS1_ULOOP(8, 16, {
    p_rd = (uint8_t)((uint16_t)p_rs1 >> insn.shamth());
})