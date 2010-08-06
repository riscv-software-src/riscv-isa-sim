require_fp;
FRC.bits = (FRA.bits &~ (uint32_t)INT32_MIN) | ((~FRB.bits) & (uint32_t)INT32_MIN);
