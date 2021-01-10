require_extension('P');
reg_t rd = (reg_t)P_W(RS1, 0) * P_W(RS2, 0);
P_64_PROFILE_END();