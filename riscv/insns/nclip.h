require_extension('P');
require_rv32;
sreg_t tmp = (sreg_t)P_RS1_PAIR >> (RS2 & 0x3f);
int32_t result = P_SAT(32, tmp);
WRITE_RD(result);