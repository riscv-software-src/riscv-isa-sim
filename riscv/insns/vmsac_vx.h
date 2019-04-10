// vmsac: vd[i] = -(x[rs1] * vs2[i]) + vd[i]
VI_VX_LOOP
({
    int64_t result = vsext(vd - ((int32_t)vd * (int32_t)rs1), sew);
    result &= ((1 << sew) - 1);
    vd = result;
})
