require_xpr64;
if(RS2 == 0)
  RD = UINT64_MAX;
else
  RD = sext32(RS1 / RS2);
