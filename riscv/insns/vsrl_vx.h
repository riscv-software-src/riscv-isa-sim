// vsrl.vx vd, vs2, rs1
VI_VX_ULOOP
({
  vd = vs2 >> (rs1 & ((1u << log2(sew)) - 1));
})
VI_CHECK_1905
