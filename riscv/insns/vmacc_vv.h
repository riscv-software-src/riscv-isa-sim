// vmacc: vd[i] = +(vs1[i] * vs2[i]) + vd[i]
VI_VV_LOOP
({
    int64_t result = (int32_t)vs1 * (int32_t)vs2 + vd;
    result &= ((1 << sew) - 1);
    vd = vsext(result, sew);
})
