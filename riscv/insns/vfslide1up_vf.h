//vfslide1up.vf vd, vs2, rs1
VI_CHECK_SLIDE(true);

VI_VFP_LOOP_BASE
if (i != 0) {
  switch (p->VU.vsew) {
    case e16: {
      VI_XI_SLIDEUP_PARAMS(e16, 1);
      vd = vs2;
    }
    break;
    case e32: {
      VI_XI_SLIDEUP_PARAMS(e32, 1);
      vd = vs2;
    }
    break;
    case e64: {
      VI_XI_SLIDEUP_PARAMS(e64, 1);
      vd = vs2;
    }
    break;
  }
} else {
  switch (p->VU.vsew) {
    case e16:
      p->VU.elt<float16_t>(rd_num, 0, true) = f16(FRS1);
      break;
    case e32:
      p->VU.elt<float32_t>(rd_num, 0, true) = f32(FRS1);
      break;
    case e64:
      p->VU.elt<float64_t>(rd_num, 0, true) = f64(FRS1);
      break;
  }
}
VI_VFP_LOOP_END
