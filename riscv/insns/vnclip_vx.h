// vnclip: vd[i] = clip(round(vs2[i] + rnd) >> rs1[i])
VRM xrm = p->VU.get_vround_mode();
uint64_t int_max = (1 << (p->VU.vsew - 1)) - 1;
uint64_t unsigned_mask = ~(-1 << (p->VU.vsew - 1));
VI_VX_LOOP
({
    uint64_t result = (vs2 & unsigned_mask) << sew; // expend the vs2 size more than SEW
    uint64_t sign = vs2 & ((uint64_t)1 << (sew - 1));
    // rounding
    INT_ROUNDING(result, xrm, sew);

	// unsigned shifting to rs1
    result = result >> rs1;
    // saturation
    if ((result & ((int64_t)-1 << sew)) != 0){
      result = sign | int_max; 
      p->VU.vxsat = 1;
	} else if (sign > 0){
	  result = vsext(result|sign, sew);
	}
    vd = result;
})
