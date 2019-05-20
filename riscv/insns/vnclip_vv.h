// vnclip: vd[i] = clip(round(vs2[i] + rnd) >> vs1[i])
VRM xrm = p->VU.get_vround_mode();
uint64_t int_max = (1 << (p->VU.vsew - 1)) - 1;
uint64_t unsigned_mask = ~(-1 << (p->VU.vsew - 1));
VI_VVXI_LOOP_NARROW
({
 
 uint64_t result = vs2;
 
 uint64_t sign = vs2 & (1ull << (sew * 2 - 1));
 // rounding
 INT_ROUNDING(result, xrm, sew);

 // unsigned shifting to rs1
 uint64_t unsigned_shift_amount = (uint64_t)(vs1 & ((1ll<<sew) - 1));
 if (unsigned_shift_amount >= (2 * sew)){
  if (sign == 0)
    result = 0;
  else
    result = -1;
 }else{
  result = result >> unsigned_shift_amount;
 }

 // saturation
 if ((result & (-1ll << sew)) != 0){
  result = (sign >> sew) | int_max;
  p->VU.vxsat = 1;
 } else if (result != 0){
  result = result | (sign >> sew);
 }
 
 vd = result;
})
VI_CHECK_1905
