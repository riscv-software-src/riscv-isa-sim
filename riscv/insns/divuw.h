require_xpr64;
reg_t lhs = zext32(RS1);
reg_t rhs = zext32(RS2);
if(rhs == 0)
  RD = UINT64_MAX;
else
  RD = sext32(lhs / rhs);
