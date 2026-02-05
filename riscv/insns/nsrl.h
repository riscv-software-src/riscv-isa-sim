require_extension('P');
require_rv32;
WRITE_RD(P_RS1_PAIR >> (RS2 & 0x3f));