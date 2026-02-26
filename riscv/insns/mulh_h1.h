require_extension('P');
require_rv32;
int64_t mres = sext(RS1,64) * sext(P_FIELD(RS2, 1, 16),64);
WRITE_RD(mres>>32);