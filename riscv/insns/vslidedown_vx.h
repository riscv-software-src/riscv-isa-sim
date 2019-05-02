//vslidedown.vx vd, vs2, rs1
VI_LOOP_BASE
  if (i + (RS1 & 0x1f) >= P.VU.vlmax)
    continue;

  if (sew == e8) {
    XI_SLIDEDOWN_PARAMS(e8, rs1);
    vd = vs2;
  } else if(sew == e16) {
    XI_SLIDEDOWN_PARAMS(e16, rs1);
    vd = vs2;
  } else if(sew == e32) {
    XI_SLIDEDOWN_PARAMS(e32, rs1);
    vd = vs2;
  } else if(sew == e64) {
    XI_SLIDEDOWN_PARAMS(e64, rs1);
    vd = vs2;
  }
VI_LOOP_END

