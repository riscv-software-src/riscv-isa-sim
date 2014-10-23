require_xpr64;
if (RS1 == p->get_state()->load_reservation)
{
  MMU.store_uint64(RS1, RS2);
  WRITE_RD(0);
}
else
  WRITE_RD(1);
