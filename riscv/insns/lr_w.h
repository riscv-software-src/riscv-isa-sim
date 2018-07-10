require_extension('A');
MMU.acquire_load_reservation(RS1);
WRITE_RD(MMU.load_int32(RS1));
