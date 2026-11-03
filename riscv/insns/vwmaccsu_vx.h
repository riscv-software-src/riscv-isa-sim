// vwmaccsu.vx vd, vs2, rs1
VI_CHECK_DSS_MACC(false);
VI_VX_LOOP_WIDEN
({
  VI_WIDE_OP_AND_ASSIGN_MIX(vs2, rs1, vd_w, *, +, int, uint, int);
})
