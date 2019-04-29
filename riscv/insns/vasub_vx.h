// vasub: Averaging subs of integers 
VRM xrm = STATE.VU.get_vround_mode();
VI_VX_LOOP
({
    int64_t result = vsext(rs1, sew) - vsext(vs2, sew); 
	INT_ROUNDING(result, xrm, 1);
	result = vzext(result >> 1, sew);

    vd = result; 
})
