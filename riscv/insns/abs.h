require_extension('P');
reg_t s1 = RS1;
reg_t result = (int64_t(s1) < 0) ? -s1 : s1;
WRITE_RD(sext_xlen(result));