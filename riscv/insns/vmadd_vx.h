// vmadd: vd[i] = (vd[i] * x[rs1]) + vs2[i]
uint64_t lower_half_mask = (1 << STATE.VU.vsew) - 1 ;
VI_VV_LOOP
({
    uint64_t result = vd * RS1;
    result &= lower_half_mask;
    vd += sext_xlen(result + vs2);
 })
