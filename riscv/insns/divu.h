reg_t lhs = zext_xprlen(RS1);
reg_t rhs = zext_xprlen(RS2);
if(rhs == 0)
  RD = UINT64_MAX;
else
  RD = sext_xprlen(lhs / rhs);
