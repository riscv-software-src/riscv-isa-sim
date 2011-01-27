if(RS2 == 0)
  RD = UINT64_MAX;
else if(sreg_t(RS1) == INT64_MIN && sreg_t(RS2) == -1)
  RD = RS1;
else
  RD = sext_xprlen(sreg_t(RS1) / sreg_t(RS2));
