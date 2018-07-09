require_extension('A');
require_rv64;
MMU.acquire_load_reservation(RS1);
WRITE_RD(MMU.load_int64(RS1));
