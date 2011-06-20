if(RS2 == 0)
  RD = RS1;
else if(sreg_t(RS1) == INT64_MIN && sreg_t(RS2) == -1)
  RD = 0;
else
  RD = sext_xprlen(sext_xprlen(RS1) % sext_xprlen(RS2));
