require_xpr64;
if(RS2 == 0)
  RD = RS1;
else if(int32_t(RS1) == INT32_MIN && int32_t(RS2) == -1)
  RD = 0;
else
  RD = sext32(int32_t(RS1) % int32_t(RS2));
