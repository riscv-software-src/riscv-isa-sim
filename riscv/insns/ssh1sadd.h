require_extension('P');
require_rv32;
WRITE_RD(sext32(P_SAT(xlen, ((RS1 << 1) + RS2))));
