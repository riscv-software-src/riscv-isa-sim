// vminu.vv vd, vs2, vs1, vm   # Vector-vector
VI_VV_LOOP
({
    vd = zext_xlen(std::min(vs1, vs2));
 })
