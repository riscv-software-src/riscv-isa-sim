require_extension('M');
reg_t lhs = zext_xlen(RS1);
reg_t rhs = zext_xlen(RS2);
if (rhs == 0)
  WRITE_RD(UINT64_MAX);
else
  WRITE_RD(sext_xlen(lhs / rhs));
