// vext_x_v: rd = vs2[0]
require(insn.v_vm() == 1);
uint64_t xmask = UINT64_MAX >> (64 - P.get_max_xlen());
VI_LOOP_BASE
VI_LOOP_END_NO_TAIL_ZERO
switch(sew) {
case e8:
  WRITE_RD(P.VU.elt<uint8_t>(rs2_num, 0));
  break;
case e16:
  WRITE_RD(P.VU.elt<uint16_t>(rs2_num, 0));
  break;
case e32:
  if (P.get_max_xlen() == 32)
    WRITE_RD(P.VU.elt<int32_t>(rs2_num, 0));
  else
    WRITE_RD(P.VU.elt<uint32_t>(rs2_num, 0));
  break;
case e64:
  if (P.get_max_xlen() <= sew)
    WRITE_RD(P.VU.elt<uint64_t>(rs2_num, 0) & xmask);
  else
    WRITE_RD(P.VU.elt<uint64_t>(rs2_num, 0));
  break;
}
