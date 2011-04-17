require_xpr64;
if(RS2 == 0)
  RD = UINT64_MAX;
else
  RD = sext32(zext32(RS1) / zext32(RS2));
