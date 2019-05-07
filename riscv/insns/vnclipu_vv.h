// vnclipu: vd[i] = clip(round(vs2[i] + rnd) >> vs1[i])
VRM xrm = p->VU.get_vround_mode();
uint64_t int_max = ~(-1ll << p->VU.vsew);
VI_VV_LOOP_NARROW
({

 uint64_t result = vs2_u;

 // rounding
 INT_ROUNDING(result, xrm, sew);
 // unsigned shifting to rs1
 result = result >> vs1;

 // saturation
 if (result & (uint64_t)(-1ll << sew)){
   result = int_max; 
   p->VU.vxsat = 1;
 }

 vd = result;
})
