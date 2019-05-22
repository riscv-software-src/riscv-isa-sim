// vnsrl.vi vd, vs2, zimm5
VI_VVXI_LOOP_NARROW
({
  vd = vs2_u >> (vzext(zimm5, sew) & ((1u << log2(sew * 2)) - 1));
})
VI_CHECK_1905
