require_extension('A');
require_rv64;

bool have_reservation = MMU.check_load_reservation(RS1);
MMU.amo_uint64(RS1, [&](uint64_t lhs) { return have_reservation ? RS2 : lhs; });
MMU.yield_load_reservation();

WRITE_RD(!have_reservation);
