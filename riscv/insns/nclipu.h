require_extension('P');
require_rv32;
reg_t tmp = (reg_t)P_RS1_PAIR >> (RS2 & 0x3f);
uint32_t result = P_USAT(32, tmp);
WRITE_RD(result);