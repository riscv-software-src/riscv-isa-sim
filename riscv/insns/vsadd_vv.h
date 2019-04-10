// vsadd: Saturating adds of signed integers
VI_VV_LOOP
({
	vd =  (vs1 < 0) ? -(1 << (sew - 1)) : (1 << (sew -1)) - 1;
	if ((vs1 < 0) == (vs2 > (vd - vs1))) vd = vs1 + vs2;
})

