// vwadd.vv vd, vs2, vs1
V_WIDE_CHECK;
VI_VV_LOOP
({
  V_WIDE_OP_AND_ASSIGN(vs2, vs1, 0, +, +, int);
})
