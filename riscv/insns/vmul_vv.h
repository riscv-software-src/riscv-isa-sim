// vmul
reg_t vsew = STATE.VU.vsew;
uint64_t lo_mask = ((uint64_t)1 << vsew) - 1;

VI_VV_LOOP
({
    int64_t result = vs1 * vs2;
	vd = result & lo_mask;
})
