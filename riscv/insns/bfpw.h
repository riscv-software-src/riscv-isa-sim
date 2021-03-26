require_rv64;
require_extension('B');
reg_t cfg = RS2 >> 16;
int len = (cfg >> 8) & 15;
int off = cfg & 31;
len = len ? len : 16;
reg_t mask = ~(~reg_t(0) << len) << off;
reg_t data = RS2 << off;
WRITE_RD(sext32((data & mask) | (RS1 & ~mask)));
