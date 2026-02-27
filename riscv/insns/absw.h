require_rv64;
require_extension('P');
reg_t s1_w = sext32(RS1);
reg_t result = (int32_t(s1_w) < 0) ? -s1_w : s1_w;
WRITE_RD(sext_xlen(result));
