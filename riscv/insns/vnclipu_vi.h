// vnclipu: vd[i] = clip(round(vs2[i] + rnd) >> simm)
VRM xrm = p->VU.get_vround_mode();
uint64_t int_max = ~(-1ll << p->VU.vsew);
VI_VI_ULOOP
({
    uint64_t result = (vs2 + 0llu) << sew; // expend the vs2 size more than SEW
    // rounding
    INT_ROUNDING(result, xrm, sew);

	// unsigned shifting to rs1
    result = result >> simm5;

    // saturation
    if (result & (uint64_t)(-1ll << sew)){
       result = int_max; 
       p->VU.vxsat = 1;
	}

    vd = result;
})
