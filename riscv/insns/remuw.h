if(uint32_t(RS2) == 0)
  RD = 0;
else
  RD = sext32(uint32_t(RS1) % uint32_t(RS2));
