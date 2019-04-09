// vext_x_v: rd = vs2[rs1]
VI_XV_LOOP({
	WRITE_RD(vs2);
	break;
})
