// vfwcvt.x.f.v vd, vs2, vm
VI_VFP_CVT_SCALE
({
  ;
},
{
  auto vs2 = P.VU.elt<float16_t>(rs2_num, i);
  P.VU.elt<int32_t>(rd_num, i, true) = f16_to_i32(vs2, STATE.frm->read(), true);
},
{
  auto vs2 = P.VU.elt<float32_t>(rs2_num, i);
  P.VU.elt<int64_t>(rd_num, i, true) = f32_to_i64(vs2, STATE.frm->read(), true);
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
