// vsadd: Saturating adds of signed integers
VI_VX_LOOP
({
	vd =  (rs1 < 0) ? -(1 << (sew - 1)) : (1 << (sew -1)) - 1;
	if ((rs1 < 0) == (vs2 > (vd - rs1))) vd = rs1 + vs2;

})
