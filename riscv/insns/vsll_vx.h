// vsll
VI_VX_LOOP
({
    vd = vsext(vs2 << (rs1 & (sew-1)), sew);
})
VI_CHECK_1905
