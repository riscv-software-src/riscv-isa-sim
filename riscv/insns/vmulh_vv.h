// vmulh
reg_t vsew = STATE.VU.vsew;
uint64_t lo_mask = ((uint64_t)1 << vsew) - 1;

VI_VV_LOOP
({
    int64_t result = (int64_t)vs1 * (int64_t)vs2;
	vd = (result >> sew) & lo_mask;
})
