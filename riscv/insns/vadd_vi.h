// vadd.vi vd, simm5, vs2, vm
VI_VI_LOOP
({
    vd = vsext(simm5 + vs2, sew);
 })
