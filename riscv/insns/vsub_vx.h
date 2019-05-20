// vsub: vd[i] = (vd[i] * x[rs1]) - vs2[i]
VI_VX_LOOP
({
    vd = vsext(vs2 - rs1, sew);
})
VI_CHECK_1905
