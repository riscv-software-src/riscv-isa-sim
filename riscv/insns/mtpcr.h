require_supervisor;
reg_t val = get_pcr(insn.rtype.rs1);
if (val == 0 || insn.rtype.rs1 != PCR_TOHOST)
  set_pcr(insn.rtype.rs1, RS2);
RD = val;
