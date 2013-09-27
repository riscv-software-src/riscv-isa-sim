reg_t lhs = zext_xprlen(RS1);
reg_t rhs = zext_xprlen(RS2);
if(rhs == 0)
  WRITE_RD(UINT64_MAX);
else
  WRITE_RD(sext_xprlen(lhs / rhs));
