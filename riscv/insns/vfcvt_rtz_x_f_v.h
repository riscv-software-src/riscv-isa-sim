// vfcvt.rtz.x.f.v vd, vd2, vm
VI_VFP_VF_LOOP
({
  p->VU.elt<int16_t>(rd_num, i) = f16_to_i16(vs2, softfloat_round_minMag, true);
},
{
  p->VU.elt<int32_t>(rd_num, i) = f32_to_i32(vs2, softfloat_round_minMag, true);
},
{
  p->VU.elt<int64_t>(rd_num, i) = f64_to_i64(vs2, softfloat_round_minMag, true);
})
