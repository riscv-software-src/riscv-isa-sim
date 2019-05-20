// vrsub.vx vd, vs2, rs1, vm   # vd[i] = rs1 - vs2[i]
VI_VX_LOOP
({
    vd = vsext(rs1 - vs2, sew);
})
VI_CHECK_1905
