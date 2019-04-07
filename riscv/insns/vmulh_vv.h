// vmulh
reg_t vsew = STATE.VU.vsew;
uint64_t lo_mask = 1 - ((1 << vsew) - 1);

VI_VV_LOOP
({
    uint64_t result = vs1 * vs2;
	vd = vsext((result >> sew) & lo_mask, sew);
})
