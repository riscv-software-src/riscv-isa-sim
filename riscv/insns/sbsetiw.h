require_extension('B');
int shamt = SHAMT & 31;
WRITE_RD(sext32(RS1 | (1LL << shamt)));
