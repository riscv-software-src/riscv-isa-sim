//vslide1down.vx vd, vs2, rs1
VI_CHECK_SLIDE(false);

VI_LOOP_BASE
if (i != vl - 1) {
  switch (sew) {
  case e8: {
    VI_XI_SLIDEDOWN_PARAMS(e8, 1);
    vd = vs2;
  }
  break;
  case e16: {
    VI_XI_SLIDEDOWN_PARAMS(e16, 1);
    vd = vs2;
  }
  break;
  case e32: {
    VI_XI_SLIDEDOWN_PARAMS(e32, 1);
    vd = vs2;
  }
  break;
  default: {
    VI_XI_SLIDEDOWN_PARAMS(e64, 1);
    vd = vs2;
  }
  break;
  }
} else {
  switch (sew) {
  case e8:
    P.VU.elt<uint8_t>(rd_num, vl - 1, true) = RS1;
    break;
  case e16:
    P.VU.elt<uint16_t>(rd_num, vl - 1, true) = RS1;
    break;
  case e32:
    P.VU.elt<uint32_t>(rd_num, vl - 1, true) = RS1;
    break;
  default:
    P.VU.elt<uint64_t>(rd_num, vl - 1, true) = RS1;
    break;
  }
}
VI_LOOP_END
