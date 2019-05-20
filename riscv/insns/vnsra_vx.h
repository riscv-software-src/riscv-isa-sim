// vnsra.vx vd, vs2, rs1
VI_VVXI_LOOP_NARROW
({
  vd = vs2 >> (rs1 & ((1u << log2(sew * 2)) - 1));
})
VI_CHECK_1905
