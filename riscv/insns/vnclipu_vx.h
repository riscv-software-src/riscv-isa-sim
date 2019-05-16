// vnclipu: vd[i] = clip(round(vs2[i] + rnd) >> rs1[i])
VRM xrm = p->VU.get_vround_mode();
uint64_t int_max = ~(-1ll << p->VU.vsew);
VI_VVXI_LOOP_NARROW
({
    uint64_t result = vs2;
    // rounding
    INT_ROUNDING(result, xrm, sew);

	// unsigned shifting to rs1
    uint64_t unsigned_shift_amount = (uint64_t)(rs1 & ((1ll<<sew) - 1));
    if (unsigned_shift_amount >= (2 * sew)){
    result = 0;
    }else{
    result = result >> unsigned_shift_amount;
    }

    // saturation
    if (result & (uint64_t)(-1ll << sew)){
       result = int_max; 
       p->VU.vxsat = 1;
    }

    vd = result;
})
