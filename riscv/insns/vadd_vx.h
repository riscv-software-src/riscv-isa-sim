// vadd.vx vd, rs1, vs2, vm
VI_VX_LOOP
({
    vd = sext_xlen(rs1 + vs2);
 })
