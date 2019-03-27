// vsrl
VI_VV_LOOP
({
    WRITE_RD(sext_xlen(zext_xlen(vs1) >> (vs2 & (xlen-1))));
})
