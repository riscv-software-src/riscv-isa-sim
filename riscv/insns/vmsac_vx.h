// vmsac: vd[i] = -(x[rs1] * vs2[i]) + vd[i]
uint64_t lower_half_mask = (1 << STATE.VU.vsew) - 1 ;
VI_VX_LOOP
({
    uint64_t result = rs1 * vs2;
    result &= lower_half_mask;
    vd -= sext_xlen(result + vd);

})
