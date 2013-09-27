sreg_t lhs = sext_xprlen(RS1);
sreg_t rhs = sext_xprlen(RS2);
if(rhs == 0)
  WRITE_RD(UINT64_MAX);
else if(lhs == INT64_MIN && rhs == -1)
  WRITE_RD(lhs);
else
  WRITE_RD(sext_xprlen(lhs / rhs));
