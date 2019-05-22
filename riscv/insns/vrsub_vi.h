// vrsub.vi vd, vs2, imm, vm   # vd[i] = imm - vs2[i]
VI_VI_LOOP
({
    vd = vsext(vzext(simm5, 5) - vs2, sew);
})
VI_CHECK_1905
