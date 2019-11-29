// vmflt.vv vd, vs2, vs1
VI_VFP_LOOP_CMP
({
  res = f32_lt(vs2, vs1);
},
{
  res = f64_lt(vs2, vs1);
},
true)
