if(xpr64)
  RD = RS1 >> (RS2 & 0x3F);
else
  RD = sext32((uint32_t)RS1 >> (RS2 & 0x1F));
