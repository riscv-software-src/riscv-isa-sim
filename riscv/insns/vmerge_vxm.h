// vmerge.vxm vd, vs2, rs1
VI_VX_MERGE_LOOP
({
  vd = use_first ? rs1 : vs2;
})
