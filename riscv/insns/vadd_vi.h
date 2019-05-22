// vadd.vi vd, simm5, vs2, vm
VI_VI_LOOP
({
    vd = vsext(vzext(simm5, 5) + vs2, sew);
 })
VI_CHECK_1905
