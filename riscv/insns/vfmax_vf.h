// vfmax

require_zvfbfa

VI_VFP_VF_LOOP
({
  vd = VFP_OP_16(max, vs2, rs1);
},
{
  vd = f32_max(vs2, rs1);
},
{
  vd = f64_max(vs2, rs1);
})
