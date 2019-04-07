// vmul
reg_t vsew = STATE.VU.vsew;
uint64_t lo_mask = 1 - ((1 << vsew) - 1);

VI_VX_LOOP
({
    uint64_t result = rs1 * vs2;
	vd = vsext(result & lo_mask, sew);
})
