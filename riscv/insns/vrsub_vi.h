// vrsub.vi vd, vs2, imm, vm   # vd[i] = imm - vs2[i]
VI_VI_LOOP
({
    vd = vsext(simm5 - vs2, sew);
})
