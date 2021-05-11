require_extension('P');
sreg_t rs1 = RS1;
sreg_t rs2 = RS2;
sreg_t carry = (rs1 & 1) | (rs2 & 1);
WRITE_RD(sext_xlen((rs1 >> 1) + (rs2 >> 1) + carry));