require_extension('P');
reg_t rc = READ_REG(insn.p_rc());
reg_t rs1 = RS1;
reg_t rs2 = RS2;

WRITE_RD(zext_xlen((rs1 & rc) | (rs2 & ~rc)));