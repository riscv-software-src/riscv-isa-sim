// vmsub: vd[i] = (vd[i] * vs1[i]) - vs2[i]
VI_VV_LOOP
({
    int64_t result = vsext(((int32_t)vd * (int32_t)vs1) - vs2, sew);
    result &= ((1 << sew) - 1);
    vd = result;
})
