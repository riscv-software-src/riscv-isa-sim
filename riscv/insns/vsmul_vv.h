// vsmul: Signed saturating and rounding fractional multiply
VRM xrm = p->VU.get_vround_mode();
int64_t int_max = (1 << p->VU.vsew) - 1;
int64_t int_min = - (1 << (p->VU.vsew - 1));
int64_t sign_mask = ((1 << (p->VU.vsew - 1)));

VI_VV_ULOOP
({
    int64_t vs1_sign;
    int64_t vs2_sign;
    int64_t result_sign;

	vs1_sign = vs1 & sign_mask;
	vs2_sign = vs2 & sign_mask;
    uint64_t result = vzext((uint64_t)vs1 * (uint64_t)vs2, sew * 2);
    printf("sew:   %d, %llX * %llX = %llX, mask: %llX\n",sew, vs1, vs2, result, (0x3 << ((sew * 2) - 2)));
    uint64_t sign_bits = (result & (0x3 << ((sew * 2) - 2))) >> ((sew * 2) - 2);
    printf("signbits=%llX\n",sign_bits);

    result_sign = vs1_sign ^ vs2_sign;
	// rounding
	INT_ROUNDING(result, xrm, sew);
    uint64_t after_sign_bits = (result & (0x3 << ((sew * 2) - 2))) >> ((sew * 2) - 2);
    printf("after signbits=%llX\n",after_sign_bits);

    // unsigned shifting to rs1
    result = result >> (sew - 1);
    printf("result=%llX\n", result);

    // saturation
	if (sign_bits != after_sign_bits){
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
