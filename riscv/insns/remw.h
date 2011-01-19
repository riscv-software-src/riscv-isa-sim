require_xpr64;
// INT64_MIN/-1 corner case shouldn't occur in correct code, since
// INT64_MIN is not a proper 32-bit signed value
if(RS2 == 0)
  RD = RS1;
else if(sreg_t(RS1) == INT64_MIN && sreg_t(RS2) == -1)
  RD = 0;
else
  RD = sext32(sreg_t(RS1) % sreg_t(RS2));
