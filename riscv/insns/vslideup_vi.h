// vslideup.vi vd, vs2, rs1
VI_LOOP_BASE
  const uint32_t uimm5 = insn.v_zimm5();
  if (i + uimm5 >= P.VU.vlmax)
    continue;

  if (sew == e8) {
    XI_SLIDEUP_PARAMS(e8, uimm5);
    vd = vs2;
  } else if(sew == e16) {
    XI_SLIDEUP_PARAMS(e16, uimm5);
    vd = vs2;
  } else if(sew == e32) {
    XI_SLIDEUP_PARAMS(e32, uimm5);
    vd = vs2;
  } else if(sew == e64) {
    XI_SLIDEUP_PARAMS(e64, uimm5);
    vd = vs2;
  }
VI_LOOP_END

