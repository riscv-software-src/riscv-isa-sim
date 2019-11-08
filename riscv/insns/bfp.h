require_extension('B');
reg_t cfg = RS2 >> (xlen/2);
if ((cfg >> 30) == 2)
	cfg = cfg >> 16;
int len = (cfg >> 8) & (xlen/2-1);
int off = cfg & (xlen-1);
len = len ? len : xlen/2;
reg_t mask = ~(~reg_t(0) << len) << off;
reg_t data = RS2 << off;
WRITE_RD(sext_xlen((data & mask) | (RS1 & ~mask)));
