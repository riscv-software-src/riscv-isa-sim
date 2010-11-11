if(uint32_t(RS2) == 0)
  RD = sext32(UINT32_MAX);
else
  RD = sext32(uint32_t(RS1)/uint32_t(RS2));
