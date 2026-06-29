require_extension('P');
require_rv32;
WRITE_RD(sext32(P_RS1_PAIR >> (RS2 & 0x3f)));
