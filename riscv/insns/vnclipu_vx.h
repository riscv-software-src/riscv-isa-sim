// vnclipu: vd[i] = clip(round(vs2[i] + rnd) >> rs1[i])
VRM xrm = STATE.VU.get_vround_mode();
uint64_t int_max = ~(-1ll << STATE.VU.vsew);
VI_VX_ULOOP
({
    uint64_t result = (vs2 + 0llu) << sew; // expend the vs2 size more than SEW
    // rounding
    INT_ROUNDING(result, xrm, sew);

	// unsigned shifting to rs1
    result = result >> rs1;

    // saturation
    if (result & (uint64_t)(-1ll << sew)){
       result = int_max; 
       STATE.VU.vxsat = 1;
	}

    vd = result;
})
