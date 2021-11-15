// vfwcvt.xu.f.v vd, vs2, vm
VI_VFP_CVT_SCALE
({
  ;
},
{
  auto vs2 = p->VU.elt<float16_t>(rs2_num, i);
  p->VU.elt<uint32_t>(rd_num, i, true) = f16_to_ui32(vs2, STATE.frm->read(), true);
},
{
  auto vs2 = p->VU.elt<float32_t>(rs2_num, i);
  p->VU.elt<uint64_t>(rd_num, i, true) = f32_to_ui64(vs2, STATE.frm->read(), true);
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
true, (p->VU.vsew >= 16))
