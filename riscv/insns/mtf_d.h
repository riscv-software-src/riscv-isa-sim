require_fp;
if(xpr64)
  FRD = RS1;
else
  FRD = (RS1 & 0x00000000FFFFFFFF) | (RS2 << 32);
