// vmsac: vd[i] = -(vs1[i] * vs2[i]) + vd[i]
VI_VV_LOOP
({
    int64_t result = vsext(vd - ((int32_t)vs1 * (int32_t)vs2), sew);
    result &= ((1 << sew) - 1);
    vd = result;
})
