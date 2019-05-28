// vnsra.vi vd, vs2, zimm5
VI_VI_LOOP_NSHIFT
({
  vd = vs2 >> (zimm5 & ((1u << log2(sew * 2)) - 1));
})
VI_CHECK_1905
