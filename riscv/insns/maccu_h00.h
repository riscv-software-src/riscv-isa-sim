require_extension('P');
require_rv32;
WRITE_RD(RD + zext(RS1, 16) * zext(RS2, 16));