// vsub.vv vd, vs1, vs2, vm
VI_VV_LOOP
({
    vd = sext_xlen(vs1 - vs2);
 })
