// vsltu: Set if less than, unsigned
VI_VV_LOOP
({
    WRITE_RD(reg_t(vs1) < reg_t(vs2));
})
