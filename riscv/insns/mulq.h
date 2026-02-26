require_extension('P');
require_rv32;
if (((int32_t)RS1 != INT32_MIN) | ((int32_t)RS2 != INT32_MIN)) {
    WRITE_RD((RS1 * RS2) >> 31);
  } else {
    WRITE_RD(INT32_MAX);
}