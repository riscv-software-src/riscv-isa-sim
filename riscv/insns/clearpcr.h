require_supervisor;
reg_t temp = get_pcr(insn.rtype.rs1);
set_pcr(insn.rtype.rs1, temp & ~SIMM);
RD = temp;
