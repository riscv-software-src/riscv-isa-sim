if(RS2 == 0)
  RD = UINT64_MAX;
else
  RD = sext_xprlen(RS1 / RS2);
