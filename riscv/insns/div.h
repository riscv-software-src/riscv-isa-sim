sreg_t lhs = sext_xprlen(RS1);
sreg_t rhs = sext_xprlen(RS2);
if(rhs == 0)
  RD = UINT64_MAX;
else if(lhs == INT64_MIN && rhs == -1)
  RD = lhs;
else
  RD = sext_xprlen(lhs / rhs);
