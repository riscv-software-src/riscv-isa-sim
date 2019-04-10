// vmadd: vd[i] = (vd[i] * vs1[i]) + vs2[i]
VI_VV_LOOP
({
    int64_t result = (int32_t)vd * (int32_t)vs1 + vs2;
    result &= ((1 << sew) - 1);
    vd = vsext(result, sew);
})
