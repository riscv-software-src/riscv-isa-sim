// vfsub.vf vd, vs2, rs1
require_zvfbfa

VI_VFP_VF_LOOP
({
  vd = VFP_OP_16(sub, vs2, rs1);
},
{
  vd = f32_sub(vs2, rs1);
},
{
  vd = f64_sub(vs2, rs1);
})
