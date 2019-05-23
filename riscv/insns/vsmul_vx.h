// vsmul
VRM xrm = p->VU.get_vround_mode();
int64_t int_max = (1 << (p->VU.vsew - 1)) - 1;
int64_t int_min = - (1 << (p->VU.vsew - 1));
int64_t sign_mask = ((1 << (p->VU.vsew - 1)));

VI_VX_ULOOP
({
 int64_t rs1_sign;
 int64_t vs2_sign;
 int64_t result_sign;

 rs1_sign = rs1 & sign_mask;
 vs2_sign = vs2 & sign_mask;
 bool overflow = rs1 == vs2 && rs1 == int_min;

 uint64_t result = vzext((uint64_t)rs1 * (uint64_t)vs2, sew * 2);
 result &= (1llu << ((sew * 2) - 2)) - 1;
 result_sign = rs1_sign ^ vs2_sign;
 // rounding
 INT_ROUNDING(result, xrm, sew - 1);

 // unsigned shifting
 result = result >> (sew - 1);

 // saturation
 if (overflow){
   result = int_max;
   p->VU.vxsat = 1;
 }else{
   result |= result_sign;
 }
 vd = result;
})
VI_CHECK_1905
