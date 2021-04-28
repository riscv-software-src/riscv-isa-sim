require_extension('B');
uint64_t c = 0, i = 0, data = zext_xlen(RS1), mask = zext_xlen(RS2);
while (mask) {
	uint64_t b = mask & ~((mask | (mask-1)) + 1);
	c |= (data << (ctz(b) - i)) & b;
	i += popcount(b);
	mask -= b;
}
WRITE_RD(sext_xlen(c));
