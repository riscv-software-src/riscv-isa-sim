require_rv64;
require_extension('B');
WRITE_RD(xperm(RS1, RS2, 5, xlen));
