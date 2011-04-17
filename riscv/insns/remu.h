if(RS2 == 0)
  RD = RS1;
else
  RD = sext_xprlen(zext_xprlen(RS1) % zext_xprlen(RS2));
