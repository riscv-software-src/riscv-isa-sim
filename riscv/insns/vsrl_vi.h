// vsrl.vi vd, vs2, zimm5
VI_VI_ULOOP
({
  vd = vs2 >> (simm5 & ((1u << log2(sew)) - 1) & 0x1f);
})
