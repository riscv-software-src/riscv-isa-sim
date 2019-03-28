// vmacc: vd[i] = +(x[rs1] * vs2[i]) + vd[i]
VI_VX_LOOP
({
    vd += sext_xlen((RS1 * vs2) + vd);
})
