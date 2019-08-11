require_extension('B');
int shamt = RS2 & 31;
WRITE_RD(sext32(1 & (RS1 >> shamt)));
