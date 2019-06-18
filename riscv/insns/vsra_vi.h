// vsra.vi vd, vs2, zimm5
VI_VI_LOOP
({
  vd = vs2 >> (simm5 & (sew - 1) & 0x1f);
})
