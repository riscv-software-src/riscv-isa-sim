require_xpr64;
if (RS1 == p->get_state()->load_reservation)
{
  MMU.store_uint64(RS1, RS2);
  RD = 0;
}
else
  RD = 1;
