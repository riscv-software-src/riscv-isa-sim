require_extension('A');
require_rv64;

bool have_reservation = MMU.check_load_reservation(RS1, 8);

if (have_reservation)
  MMU.store_uint64(RS1, RS2);

MMU.yield_load_reservation();

WRITE_RD(!have_reservation);
