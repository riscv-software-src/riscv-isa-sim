// vsmul: Signed saturating and rounding fractional multiply
VRM xrm = p->VU.get_vround_mode();
int64_t int_max = (1 << p->VU.vsew) - 1;
int64_t int_min = - (1 << (p->VU.vsew - 1));
int64_t val_mask = ((1 << (p->VU.vsew - 1)) - 1);
int64_t sign_mask = ((1 << (p->VU.vsew - 1)));

VI_VV_LOOP
({
    int64_t vs1_sign;
    int64_t vs2_sign;
    int64_t result_sign;

	vs1_sign = vs1 & sign_mask;
	vs2_sign = vs2 & sign_mask;
	vs1 = vs1 & val_mask;
	vs2 = vs2 & val_mask;
    uint64_t result = vzext((uint64_t)vs1 * (uint64_t)vs2, sew * 2);
	result_sign = vs1_sign ^ vs2_sign;
	// rounding
	INT_ROUNDING(result, xrm, sew);
	// checking the overflow
	uint64_t overflow = result & ((uint64_t)-1 << ((sew - 1) * 2));
    // unsigned shifting to rs1
    result = result >> (sew - 1);
	result &= val_mask;
	result |= result_sign;

    // saturation
	if (overflow){
		if (result_sign == 0){ // positive
			result = int_max;
		}else{
			result = int_min;
		}
        p->VU.vxsat = 1;
	}
    vd = result;
})
