// vfmul.vf vd, vs2, rs1, vm
require_zvfbfa

VI_VFP_VF_LOOP
({
  vd = VFP_OP_16(mul, vs2, rs1);
},
{
  vd = f32_mul(vs2, rs1);
},
{
  vd = f64_mul(vs2, rs1);
})
