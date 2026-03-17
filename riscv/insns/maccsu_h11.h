require_extension('P');
require_rv32;
WRITE_RD(RD + sext(RS1 >> 16, 16) * zext(RS2 >> 16, 16));