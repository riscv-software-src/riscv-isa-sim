require_extension('P');
require_rv32;
WRITE_RD(RD + sext(RS1, 16) * sext(RS2 >> 16, 16));