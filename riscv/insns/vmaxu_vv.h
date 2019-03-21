// vmaxu.vv vd, vs2, vs1, vm   # Vector-vector
VI_VV_LOOP
({
    vd = zext_xlen(std::max(vs1, vs2));
 })
