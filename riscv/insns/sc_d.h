require_extension('A');
require_rv64;
if (MMU.check_load_reservation(RS1))
{
  MMU.store_uint64(RS1, RS2);
  WRITE_RD(0);
}
else
  WRITE_RD(1);

MMU.yield_load_reservation();
