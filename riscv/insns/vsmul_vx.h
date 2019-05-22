// vsmul
VRM xrm = p->VU.get_vround_mode();
int64_t int_max = (1 << (p->VU.vsew - 1)) - 1;
int64_t int_min = - (1 << (p->VU.vsew - 1));
int64_t sign_mask = ((1 << (p->VU.vsew - 1)));

VI_VX_ULOOP
({
    int64_t vs1_sign;
    int64_t vs2_sign;
    int64_t result_sign;

	vs1_sign = rs1 & sign_mask;
	vs2_sign = vs2 & sign_mask;
    uint64_t result = vzext((uint64_t)rs1 * (uint64_t)vs2, sew * 2);
    uint64_t sign_bits = (result & (0x3llu << ((sew * 2) - 2)));

    result_sign = vs1_sign ^ vs2_sign;
	// rounding
	INT_ROUNDING(result, xrm, sew - 1);
    uint64_t after_sign_bits = (result & (0x3llu << ((sew * 2) - 2)));

    // unsigned shifting to rs1
    result = result >> (sew - 1);

    // saturation
	if (sign_bits != after_sign_bits || ((-1ll << sew) & result) != 0){
		if (result_sign == 0){ // positive
			result = int_max;
		}else{
			result = int_min;
		}
        p->VU.vxsat = 1;
    }else{
	  result |= result_sign;
    }
    vd = result;

})
VI_CHECK_1905
