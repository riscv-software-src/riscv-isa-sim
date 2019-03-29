// vwmsac.vv vd, vs2, vs1
V_WIDE_CHECK;
VI_VX_LOOP
({
  V_WIDE_OP_AND_ASSIGN(vs2, vs1, vd, *, -, int);
})
