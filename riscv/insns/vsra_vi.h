// vsra.vi vd, vs2, zimm5
VI_VI_LOOP
({
  vd = vs2 >> ((uint8_t)simm5 & ((1u << log2(sew)) - 1));
})
