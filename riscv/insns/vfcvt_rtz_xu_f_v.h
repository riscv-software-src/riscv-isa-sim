// vfcvt.rtz.xu.f.v vd, vd2, vm
VI_VFP_VF_LOOP
({
  p->VU.elt<uint16_t>(rd_num, i) = f16_to_ui16(vs2, softfloat_round_minMag, true);
},
{
  p->VU.elt<uint32_t>(rd_num, i) = f32_to_ui32(vs2, softfloat_round_minMag, true);
},
{
  p->VU.elt<uint64_t>(rd_num, i) = f64_to_ui64(vs2, softfloat_round_minMag, true);
})
