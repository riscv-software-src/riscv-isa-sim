//vslideup.vx vd, vs2, rs1
VI_LOOP_BASE
  const uint32_t offset = i + RS1;
  if (offset >= P.VU.vlmax)
    continue;

  if (sew == e8) {
    XI_SLIDEUP_PARAMS(e8, offset);
    vd = vs2;
  } else if(sew == e16) {
    XI_SLIDEUP_PARAMS(e16, offset);
    vd = vs2;
  } else if(sew == e32) {
    XI_SLIDEUP_PARAMS(e32, offset);
    vd = vs2;
  } else if(sew == e64) {
    XI_SLIDEUP_PARAMS(e64, offset);
    vd = vs2;
  }
VI_LOOP_END
