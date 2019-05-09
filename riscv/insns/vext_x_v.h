// vext_x_v: rd = vs2[rs1]
uint64_t xmask = UINT64_MAX >> (64 - P.get_max_xlen());
reg_t rs1 = RS1;
VI_LOOP_BASE 
if (!(rs1 >= 0 && rs1 < P.VU.vlmax)) {
  WRITE_RD(0);
} else {
  switch(sew) {
  case e8:
    WRITE_RD(P.VU.elt<uint8_t>(rs2_num, rs1));
    break;
  case e16:
    if (P.get_max_xlen() <= sew)
      WRITE_RD(P.VU.elt<uint16_t>(rs2_num, rs1) & xmask);
    else
      WRITE_RD(vzext(P.VU.elt<uint16_t>(rs2_num, rs1), P.get_max_xlen()));
    break;
  case e32:
    if (P.get_max_xlen() <= sew)
      WRITE_RD(P.VU.elt<uint32_t>(rs2_num, rs1) & xmask);
    else
      WRITE_RD(vzext(P.VU.elt<uint32_t>(rs2_num, rs1), P.get_max_xlen()));
    break;
  case e64:
    if (P.get_max_xlen() <= sew)
      WRITE_RD(P.VU.elt<uint64_t>(rs2_num, rs1) & xmask);
    else
      WRITE_RD(vzext(P.VU.elt<uint64_t>(rs2_num, rs1), P.get_max_xlen()));
    break;
  }
}
break;
VI_LOOP_END
