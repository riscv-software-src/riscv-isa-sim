// vsrl
VI_VI_LOOP
({
    WRITE_RD(sext_xlen(zext_xlen(simm5) >> (vs2 & (xlen-1))));
})
