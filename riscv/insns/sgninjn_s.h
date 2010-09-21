require_fp;
FRDR = (FRS1 &~ (uint32_t)INT32_MIN) | ((~FRS2) & (uint32_t)INT32_MIN);
