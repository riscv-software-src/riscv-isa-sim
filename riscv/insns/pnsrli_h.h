require_rv32;
P_NARROW_RD_RS1_ULOOP(16, 32, {
    p_rd = (uint16_t)((uint32_t)p_rs1 >> insn.shamtw());
})