// vwmacc.vx vd, vs2, rs1
V_WIDE_CHECK;
VI_VV_LOOP
({
  V_WIDE_OP_AND_ASSIGN(vs2, rs1, vd, *, +, int);
})
