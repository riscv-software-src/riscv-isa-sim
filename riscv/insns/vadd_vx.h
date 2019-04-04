// vadd.vx vd, rs1, vs2, vm
VI_VX_LOOP
({
    vd = vsext(rs1 + vs2, sew);
 })
