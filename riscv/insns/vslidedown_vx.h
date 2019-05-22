//vslidedown.vx vd, vs2, rs1
VI_LOOP_BASE
  bool is_valid = (i + RS1) < P.VU.vlmax;
  reg_t offset = 0;

  if (is_valid) {
    offset = RS1;
  }

  switch (sew) {
  case e8: {
    VI_XI_SLIDEDOWN_PARAMS(e8, offset);
    vd = is_valid ? vs2 : 0;
    }
    break;
  case e16: {
    VI_XI_SLIDEDOWN_PARAMS(e16, offset);
    vd = is_valid ? vs2 : 0;
    }
    break;
  case e32: {
    VI_XI_SLIDEDOWN_PARAMS(e32, offset);
    vd = is_valid ? vs2 : 0;
    }
    break;
  default: {
    VI_XI_SLIDEDOWN_PARAMS(e64, offset);
    vd = is_valid ? vs2 : 0;
    }
    break;
  }
VI_LOOP_END
