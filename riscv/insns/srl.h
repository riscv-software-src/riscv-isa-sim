if(xpr64)
  WRITE_RD(RS1 >> (RS2 & 0x3F));
else
  WRITE_RD(sext32((uint32_t)RS1 >> (RS2 & 0x1F)));
