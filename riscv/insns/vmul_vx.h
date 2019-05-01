// vmul
reg_t vsew = p->VU.vsew;
uint64_t lo_mask = ((uint64_t)1 << vsew) - 1;

VI_VX_LOOP
({
    int64_t result = rs1 * vs2;
	vd = result & lo_mask;
})
