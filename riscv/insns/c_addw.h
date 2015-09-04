require_extension('C');
require_rv64;
WRITE_RD(sext32(RVC_RS1 + RVC_RS2));
