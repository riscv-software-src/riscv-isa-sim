// vwmacc.vx vd, vs2, rs1
VI_CHECK_DSS(false);
VI_VX_LOOP_WIDEN
({
  V_WIDE_OP_AND_ASSIGN(vs2, rs1, vd_w, *, +, int);
})
VI_CHECK_1905
