// vfncvt.f.xu.v vd, vs2, vm
VI_VFP_CVT_SCALE
({
  ;
},
{
  auto vs2 = p->VU.elt<uint32_t>(rs2_num, i);
  p->VU.elt<float16_t>(rd_num, i, true) = ui32_to_f16(vs2);
},
{
  auto vs2 = p->VU.elt<uint64_t>(rs2_num, i);
  p->VU.elt<float32_t>(rd_num, i, true) = ui64_to_f32(vs2);
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
false, (p->VU.vsew >= 16))
