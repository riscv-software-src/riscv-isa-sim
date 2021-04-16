require_extension('P');
reg_t rc = RS3;
reg_t rs1 = RS1;
reg_t rs2 = RS2;

WRITE_RD(sext_xlen((rs1 & rc) | (rs2 & ~rc)));