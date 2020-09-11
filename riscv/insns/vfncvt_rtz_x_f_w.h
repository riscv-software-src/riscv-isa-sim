// vfncvt.rtz.x.f.w vd, vs2, vm
VI_VFP_CVT_SCALE
({
  auto vs2 = P.VU.elt<float16_t>(rs2_num, i);
  P.VU.elt<int8_t>(rd_num, i, true) = f16_to_i8(vs2, softfloat_round_minMag, true);
},
{
  auto vs2 = P.VU.elt<float32_t>(rs2_num, i);
  P.VU.elt<int16_t>(rd_num, i, true) = f32_to_i16(vs2, softfloat_round_minMag, true);
},
{
  auto vs2 = P.VU.elt<float64_t>(rs2_num, i);
  P.VU.elt<int32_t>(rd_num, i, true) = f64_to_i32(vs2, softfloat_round_minMag, true);
},
{
  require(p->supports_extension(EXT_ZFH));
},
{
  require(p->supports_extension('F'));
},
{
  require(p->supports_extension('D'));
},
false, (P.VU.vsew <= 32))
