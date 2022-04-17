require_rv64;
require_extension(EXT_XZBP);
WRITE_RD(xperm(RS1, RS2, 5, xlen));
