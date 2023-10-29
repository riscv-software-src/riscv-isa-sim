//vfslide1down.vf vd, vs2, rs1
VI_CHECK_SLIDE(false);

VI_VFP_LOOP_BASE
if (0 == P.VU.vta && i >= vl) { \
  continue; \
} \
if ((true == skip && 1 == P.VU.vma && i < vl) || (1 == P.VU.vta && i >= vl)) \
  mata_action = 2; \
else \
  mata_action = 1; \
if (i != vl - 1) {
  switch (P.VU.vsew) {
    case e16: {
      VI_XI_SLIDEDOWN_PARAMS(e16, 1);
      if (1 == mata_action) \
        vd = vs2;
      else \
        *((type_sew_t<e16>::type *)&vd)  = 0xFFFF; \
    }
    break;
    case e32: {
      VI_XI_SLIDEDOWN_PARAMS(e32, 1);
      if (1 == mata_action) \
        vd = vs2;
      else \
        *((type_sew_t<e32>::type *)&vd)  = 0xFFFFFFFF; \
    }
    break;
    case e64: {
      VI_XI_SLIDEDOWN_PARAMS(e64, 1);
      if (1 == mata_action) \
        vd = vs2;
      else \
        *((type_sew_t<e64>::type *)&vd)  = 0xFFFFFFFFFFFFFFFF; \
    }
    break;
  }
} else {
  switch (P.VU.vsew) {
    case e16:
      if (1 == mata_action) \
        P.VU.elt<float16_t>(rd_num, vl - 1, true) = FRS1_H;\
      else \
        P.VU.elt<type_sew_t<e16>::type>(rd_num, vl - 1, true) = 0xFFFF; \
      break;
    case e32:
      if (1 == mata_action) \
        P.VU.elt<float32_t>(rd_num, vl - 1, true) = FRS1_F;
      else \
        P.VU.elt<type_sew_t<e32>::type>(rd_num, vl - 1, true) = 0xFFFFFFFF; \
      break;
    case e64:
      if (1 == mata_action) \
        P.VU.elt<float64_t>(rd_num, vl - 1, true) = FRS1_D;
      else \
        P.VU.elt<type_sew_t<e64>::type>(rd_num, vl - 1, true) = 0xFFFFFFFFFFFFFFFF; \
      break;
  }
}
VI_VFP_LOOP_END
