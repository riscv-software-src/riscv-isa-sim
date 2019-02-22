// vmerge.vv vd, vs1, vs2, vm
// No predication for now. Need loop that doesn't pre-predicate later.
VI_VV_LOOP
({
  vd = 1 ? vs1 : vs2;
 })
