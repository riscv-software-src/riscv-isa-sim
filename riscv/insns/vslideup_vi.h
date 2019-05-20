// vslideup.vi vd, vs2, rs1
VI_LOOP_BASE
  const uint32_t offset = i + insn.v_zimm5();
  if (i + offset >= P.VU.vlmax)
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
VI_CHECK_1905
