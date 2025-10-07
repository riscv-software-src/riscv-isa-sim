// vfmin vd, vs2, rs1

require_zvfbfa

VI_VFP_VF_LOOP
({
  vd = VFP_OP_16(min, vs2, rs1);
},
{
  vd = f32_min(vs2, rs1);
},
{
  vd = f64_min(vs2, rs1);
})
