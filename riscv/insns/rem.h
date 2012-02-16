sreg_t lhs = sext_xprlen(RS1);
sreg_t rhs = sext_xprlen(RS2);
if(rhs == 0)
  RD = lhs;
else if(lhs == INT64_MIN && rhs == -1)
  RD = 0;
else
  RD = sext_xprlen(lhs % rhs);
