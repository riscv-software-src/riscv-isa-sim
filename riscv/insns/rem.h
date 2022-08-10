require_extension('M');
sreg_t lhs = sext_xlen(RS1);
sreg_t rhs = sext_xlen(RS2);
if (rhs == 0)
  WRITE_RD(lhs);
else if (lhs == INT64_MIN && rhs == -1)
  WRITE_RD(0);
else
  WRITE_RD(sext_xlen(lhs % rhs));
