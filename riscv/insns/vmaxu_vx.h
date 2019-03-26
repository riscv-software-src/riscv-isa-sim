// vmaxu.vx vd, vs2, rs1, vm   # vector-scalar
VI_VX_LOOP
({
    vd = zext_xlen(std::max(rs1, vs2));
 })
