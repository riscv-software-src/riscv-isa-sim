// vmul
VI_VV_LOOP
({
    WRITE_RD(sext_xlen(vs1 * vs2));
})
