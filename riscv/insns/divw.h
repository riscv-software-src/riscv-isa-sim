require_xpr64;
if(int32_t(RS2) == 0)
  RD = UINT64_MAX;
else if(int32_t(RS1) == INT32_MIN && int32_t(RS2) == -1)
  RD = RS1;
else
  RD = sext32(int32_t(RS1) / int32_t(RS2));
