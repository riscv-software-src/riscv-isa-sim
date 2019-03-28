// vmsac: vd[i] = -(vs1[i] * vs2[i]) + vd[i]
uint64_t lower_half_mask = (1 << STATE.VU.vsew) - 1 ;
VI_VV_LOOP
({
    uint64_t result = vs1 * vs2;
    result &= lower_half_mask;
    vd -= sext_xlen(result + vd);

 })
