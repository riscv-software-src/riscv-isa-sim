// vmsub: vd[i] = (vd[i] * x[rs1]) - vs2[i]
VI_VX_LOOP
({
    int64_t result = (vd * rs1) - vs2;
	vd = vsext(result, sew) & ((1 << sew) - 1);
 })
