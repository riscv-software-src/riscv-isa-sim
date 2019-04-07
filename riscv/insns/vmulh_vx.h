// vmulh
VI_VX_LOOP
({
	vd = vsext((rs1 * vs2) >> (sew/2), sew);
})
