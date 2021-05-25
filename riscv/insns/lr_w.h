require_extension('A');
auto res = MMU.load_int32(RS1, true);
MMU.acquire_load_reservation(RS1, 4);
WRITE_RD(res);
