require_extension('P');
require_rv32;
WRITE_RD(sext32((RS1 + RS2)>>1));
