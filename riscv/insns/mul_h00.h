require_extension('P');
require_rv32;
WRITE_RD(sext(RS1, 16) * sext(RS2, 16));