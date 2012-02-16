require_xpr64;
sreg_t lhs = sext32(RS1);
sreg_t rhs = sext32(RS2);
if(rhs == 0)
  RD = lhs;
else
  RD = sext32(lhs % rhs);
