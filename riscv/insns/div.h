if(RS2 == 0 || (sreg_t(RS1) == INT64_MIN && sreg_t(RS2) == -1))
{
  if(xpr64)
    RD = sreg_t(RS1) < 0 ? INT64_MIN : INT64_MAX;
  else
    RD = sreg_t(RS1) < 0 ? sext32(INT32_MIN) : INT32_MAX;
}
else
  RD = sext_xprlen(sreg_t(RS1) / sreg_t(RS2));
