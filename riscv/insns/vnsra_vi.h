// vnsra.vi vd, vs2, zimm5
VI_VI_LOOP_NSHIFT
({
  vd = vs2 >> (zimm5 & (sew * 2 - 1) & 0x1f);
})
