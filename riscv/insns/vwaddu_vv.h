// vwaddu vd, vs2, vs1
VI_WIDE_CHECK_TWO;
VI_VV_LOOP
({
  V_WIDE_OP_AND_ASSIGN(vs2, vs1, 0, +, +, uint);
})
