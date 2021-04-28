// vfwcvt.f.f.v vd, vs2, vm
VI_VFP_CVT_SCALE
({
  ;
},
{
  auto vs2 = P.VU.elt<float16_t>(rs2_num, i);
  P.VU.elt<float32_t>(rd_num, i, true) = f16_to_f32(vs2);
},
{
  auto vs2 = P.VU.elt<float32_t>(rs2_num, i);
  P.VU.elt<float64_t>(rd_num, i, true) = f32_to_f64(vs2);
},
{
  ;
},
{
  require(p->supports_extension(EXT_ZFH));
},
{
  require(p->supports_extension('D'));
},
true, (P.VU.vsew >= 16))
