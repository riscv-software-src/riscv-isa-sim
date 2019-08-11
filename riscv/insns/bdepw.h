require_rv64;
require_extension('B');
uint64_t c = 0, i = 0, data = zext32(RS1), mask = zext32(RS2);
while (mask) {
	uint64_t b = mask & ~((mask | (mask-1)) + 1);
	c |= (data << (__builtin_ctzl(b) - i)) & b;
	i += __builtin_popcountl(b);
	mask -= b;
}
WRITE_RD(sext32(c));
