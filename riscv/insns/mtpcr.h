require_supervisor;
reg_t val = get_pcr(insn.rtype.rs1);
set_pcr(insn.rtype.rs1, RS2);
RD = val;
