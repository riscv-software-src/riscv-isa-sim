// vmadd: vd[i] = (vd[i] * x[rs1]) + vs2[i]
VI_VX_LOOP
({
    int64_t result = ((int32_t)vd * (int32_t)rs1) + vs2;
    result &= ((1 << sew) - 1);
    vd = vsext(result, sew);
})
