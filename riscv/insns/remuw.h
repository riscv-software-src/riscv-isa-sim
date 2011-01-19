require_xpr64;
if(RS2 == 0)
  RD = RS1;
else
  RD = sext32(RS1 % RS2);
