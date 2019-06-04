// vmv.v.i vd, simm5
VI_VVXI_MERGE_LOOP
({
  vd = vsext(simm5, sew);
})
