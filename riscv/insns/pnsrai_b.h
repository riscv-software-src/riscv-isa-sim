require_rv32;
P_NARROW_RD_RS1_LOOP(8, 16, {
    p_rd = p_rs1 >> insn.shamth();
})