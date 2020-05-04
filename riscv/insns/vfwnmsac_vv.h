// vfwnmsac.vv vd, vs2, vs1
VI_VFP_VV_LOOP_WIDE
({
  vd = f32_mulAdd(f32(vs1.v ^ F32_SIGN), vs2, vd);
},
{
  vd = f64_mulAdd(f64(vs1.v ^ F64_SIGN), vs2, vd);
})
