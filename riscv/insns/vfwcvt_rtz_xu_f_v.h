// vfwcvt.rtz,xu.f.v vd, vs2, vm
VI_VFP_CVT_SCALE
({
  ;
},
{
  auto vs2 = P.VU.elt<float16_t>(rs2_num, i);
  P.VU.elt<uint32_t>(rd_num, i, true) = f16_to_ui32(vs2, softfloat_round_minMag, true);
},
{
  auto vs2 = P.VU.elt<float32_t>(rs2_num, i);
  P.VU.elt<uint64_t>(rd_num, i, true) = f32_to_ui64(vs2, softfloat_round_minMag, true);
},
{
  ;
},
{
  require(p->extension_enabled(EXT_ZFH));
},
{
  require(p->extension_enabled('F'));
},
true, (P.VU.vsew >= 16))
