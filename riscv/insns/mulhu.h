if(xpr64)
  RD = (uint128_t(RS1) * uint128_t(RS2)) >> 64;
else
  RD = sext32((RS1 * RS2) >> 32);
