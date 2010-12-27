require64;
if(RS2 == 0 || (sreg_t(RS1) == INT64_MIN && sreg_t(RS2) == -1))
  RD = 0;
else
  RD = sreg_t(RS1) % sreg_t(RS2);
