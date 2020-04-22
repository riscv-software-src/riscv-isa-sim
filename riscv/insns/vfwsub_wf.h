// vfwsub.wf vd, vs2, rs1
VI_VFP_WF_LOOP_WIDE
({
  vd = f32_sub(vs2, rs1);
},
{
  vd = f64_sub(vs2, rs1);
})
