// vmerge.vim vd, vs2, simm5
VI_VVXI_MERGE_LOOP
({
  int midx = (P.VU.vmlen * i) / 64;
  int mpos = (P.VU.vmlen * i) % 64;
  bool use_first = (P.VU.elt<uint64_t>(0, midx) >> mpos) & 0x1;

  vd = use_first ? simm5 : vs2;
})
