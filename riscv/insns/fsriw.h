require_rv64;
require_extension('B');
int shamt = SHAMT & 63;
reg_t a = RS1, b = RS3;
if (shamt >= 32) {
	a = RS3, b = RS1;
	shamt -= 32;
}
int rshamt = -shamt & 31;
WRITE_RD(sext32(shamt ? (b << rshamt) | (zext32(a) >> shamt) : a));
