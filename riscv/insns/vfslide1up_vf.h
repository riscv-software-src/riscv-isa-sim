//vfslide1up.vf vd, vs2, rs1
VI_CHECK_SLIDE(true);

VI_VFP_LOOP_BASE(1)
if (0 == P.VU.vta && i >= vl) { \
  continue; \
} \
if ((true == skip && 1 == P.VU.vma && i < vl) || (i >= vl)) \
  mata_action = 2; \
else \
  mata_action = 1; \
if (i != 0) {
  switch (P.VU.vsew) {
    case e16: {
      VI_XI_SLIDEUP_PARAMS(e16, 1);
      if (1 == mata_action) \
        vd = vs2;
      else \
        vd = vector_agnostic(vd);
    }
    break;
    case e32: {
      VI_XI_SLIDEUP_PARAMS(e32, 1);
      if (1 == mata_action) \
        vd = vs2;
      else \
        vd = vector_agnostic(vd);
    }
    break;
    case e64: {
      VI_XI_SLIDEUP_PARAMS(e64, 1);
      if (1 == mata_action) \
        vd = vs2;
      else \
        vd = vector_agnostic(vd);
    }
    break;
  }
} else {
  switch (P.VU.vsew) {
    case e16:
      if (1 == mata_action) \
        P.VU.elt<float16_t>(rd_num, 0, true) = FRS1_H;
      else \
        P.VU.elt<float16_t>(rd_num, 0, true) = vector_agnostic(P.VU.elt<float16_t>(rd_num, 0, false)); \
      break;
    case e32:
      if (1 == mata_action) \
        P.VU.elt<float32_t>(rd_num, 0, true) = FRS1_F;
      else \
        P.VU.elt<float32_t>(rd_num, 0, true) = vector_agnostic(P.VU.elt<float32_t>(rd_num, 0, false)); \
      break;
    case e64:
      if (1 == mata_action) \
        P.VU.elt<float64_t>(rd_num, 0, true) = FRS1_D;
      else \
        P.VU.elt<float64_t>(rd_num, 0, true) = vector_agnostic(P.VU.elt<float64_t>(rd_num, 0, false)); \
      break;
  }
}
VI_VFP_LOOP_END
