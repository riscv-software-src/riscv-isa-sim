require_xpr64;
if(RS2 == 0)
  RD = UINT64_MAX;
// INT64_MIN/-1 corner case shouldn't occur in correct code, since
// INT64_MIN is not a proper 32-bit signed value
else if(sreg_t(RS1) == INT64_MIN && sreg_t(RS2) == -1)
  RD = RS1;
else
  RD = sext32(sreg_t(RS1) / sreg_t(RS2));
