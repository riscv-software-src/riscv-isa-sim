if(RS2 == 0)
  RD = UINT64_MAX;
else
  RD = sext_xprlen(zext_xprlen(RS1) / zext_xprlen(RS2));
