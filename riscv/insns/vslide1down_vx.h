//vslide1down.vx vd, vs2, rs1
VI_LOOP_BASE
  if (i != vl - 1) {
    if (sew == e8) {
      XI_SLIDEDOWN_PARAMS(e8, 1);
      vd = vs2;
    } else if(sew == e16) {
      XI_SLIDEDOWN_PARAMS(e16, 1);
      vd = vs2;
    } else if(sew == e32) {
      XI_SLIDEDOWN_PARAMS(e32, 1);
      vd = vs2;
    } else if(sew == e64) {
      XI_SLIDEDOWN_PARAMS(e64, 1);
      vd = vs2;
    }
  } else {
    if (sew == e8) {
      P.VU.elt<uint8_t>(rd_num, vl - 1) = RS1;
    } else if(sew == e16) {
      P.VU.elt<uint16_t>(rd_num, vl - 1) = RS1;
    } else if(sew == e32) {
      P.VU.elt<uint32_t>(rd_num, vl - 1) = RS1;
    } else if(sew == e64) {
      P.VU.elt<uint64_t>(rd_num, vl - 1) = RS1;
    }
  }
VI_LOOP_END

