// vnsrl.vx vd, vs2, rs1
VI_VX_LOOP_NSHIFT
({
  vd = vs2_u >> (rs1 & ((1u << log2(sew * 2)) - 1));
})
VI_CHECK_1905
