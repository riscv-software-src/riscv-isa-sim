// vnclipu: vd[i] = clip(round(vs2[i] + rnd) >> simm)
VRM xrm = p->VU.get_vround_mode();
uint64_t int_max = ~(-1ll << p->VU.vsew);
VI_VVXI_LOOP_NARROW
({
    uint64_t result = vs2_u;
    // rounding
    INT_ROUNDING(result, xrm, sew);

	// unsigned shifting to rs1
    result = result >> zimm5;

    // saturation
    if (result & (uint64_t)(-1ll << sew)){
       result = int_max; 
       p->VU.vxsat = 1;
    }

    vd = result;
})
VI_CHECK_1905
