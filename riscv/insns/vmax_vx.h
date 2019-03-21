// vmax.vx vd, vs2, rs1, vm   # vector-scalar
VI_VX_LOOP
({
    vd = sext_xlen(std::max(vs1, rs2));
 })
