require_rvc;
if(cmp_trunc(CRS1S) != cmp_trunc(CRS2S))
  set_pc(CBRANCH_TARGET);
