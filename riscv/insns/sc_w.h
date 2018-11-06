require_extension('A');

bool have_reservation = MMU.check_load_reservation(RS1);
MMU.amo_uint32(RS1, [&](uint32_t lhs) { return have_reservation ? RS2 : lhs; });
MMU.yield_load_reservation();

WRITE_RD(!have_reservation);
