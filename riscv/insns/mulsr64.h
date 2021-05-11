require_extension('P');
sreg_t rd = (sreg_t)P_SW(RS1, 0) * P_SW(RS2, 0);
P_64_PROFILE_END();