// vadd.vi vd, simm5, vs2, vm
VI_VI_LOOP
({
    vd = vsext(simm5, sew) + vs2;
 })
VI_CHECK_1905
