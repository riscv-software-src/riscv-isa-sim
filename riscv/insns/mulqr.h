require_extension('P');
require_rv32;
if ((RS1 != INT32_MIN) | (RS2 != INT32_MIN)) {
    WRITE_RD((((RS1 * RS2) >> 30) + 1) >> 1);
  } else {
    WRITE_RD(INT32_MAX);
}