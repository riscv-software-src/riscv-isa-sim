require_rv64;
require_extension('B');
WRITE_RD(sext32(~((~RS1) << SHAMT)));
