if(xpr64)
  RD = (uint128_t(RS1) * uint128_t(RS2)) >> 64;
else
  RD = sext32(((uint64_t)(uint32_t)RS1 * (uint64_t)(uint32_t)RS2) >> 32);
