require_fp;
FRC = (FRA &~ (uint32_t)INT32_MIN) | (FRB & (uint32_t)INT32_MIN);
