// vadd.vi vd, simm5, vs2, vm
VI_VI_LOOP
({
    vd = sext_xlen(simm5 + vs2);
 })
