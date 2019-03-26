// vsle: Set if less than or equal, signed
VI_VV_LOOP
({
    WRITE_RD(sreg_t(vs1) <= sreg_t(vs2));
})
