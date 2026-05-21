require_extension('P');
require_rv32;
int64_t tmp = (((int128_t)(int32_t)RS1 * (int32_t)RS2) + (1 << 30)) >> 31;
WRITE_P_RD_PAIR(P_RD_PAIR + tmp);
