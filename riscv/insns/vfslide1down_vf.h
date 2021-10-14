//vfslide1down.vf vd, vs2, rs1
VI_CHECK_SLIDE(false);

VI_VFP_LOOP_BASE
if (i != vl - 1) {
  switch (P.VU.vsew) {
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
    case e64: {
      VI_XI_SLIDEDOWN_PARAMS(e64, 1);
      vd = vs2;
    }
    break;
  }
} else {
  switch (P.VU.vsew) {
    case e16:
      P.VU.elt<float16_t>(rd_num, vl - 1, true) = FRS1_H;
      break;
    case e32:
      P.VU.elt<float32_t>(rd_num, vl - 1, true) = FRS1_F;
      break;
    case e64:
      P.VU.elt<float64_t>(rd_num, vl - 1, true) = FRS1_D;
      break;
  }
}
VI_VFP_LOOP_END
