require_extension('B');
int shamt = RS2 & (2*xlen-1);
reg_t a = RS1, b = RS3;
if (shamt >= xlen) {
	a = RS3, b = RS1;
	shamt -= xlen;
}
int rshamt = -shamt & (xlen-1);
WRITE_RD(sext_xlen(shamt ? (b << rshamt) | (zext_xlen(a) >> shamt) : a));
