// vsqrt.v vd, vd2, vm
VI_NON_ALTFMT_INSN

VI_VFP_V_LOOP
({
  vd = f16_sqrt(vs2);
},
{
  vd = f32_sqrt(vs2);
},
{
  vd = f64_sqrt(vs2);
})
