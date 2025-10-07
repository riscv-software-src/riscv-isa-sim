// vfadd.vf vd, vs2, rs1
require_zvfbfa

VI_VFP_VF_LOOP
({
  vd = VFP_OP_16(add, vs2, rs1);
},
{
  vd = f32_add(rs1, vs2);
},
{
  vd = f64_add(rs1, vs2);
})
