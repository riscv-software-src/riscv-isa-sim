// vaadd: Averaging adds of integers 
VRM xrm = p->VU.get_vround_mode();
VI_VV_LOOP
({
    int64_t result = vsext(vs1, sew) + vsext(vs2, sew); 
	INT_ROUNDING(result, xrm, 1);
    result = vzext(result >> 1, sew);

    vd = result; 
})
