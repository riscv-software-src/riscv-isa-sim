// vmacc: vd[i] = +(x[rs1] * vs2[i]) + vd[i]
VI_VX_LOOP
({
    int64_t result = (int32_t)rs1 * (int32_t)vs2 + vd;
    result &= ((1 << sew) - 1);
    vd = vsext(result, sew);
})
