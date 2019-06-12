// vfsub.vf vd, vs2, rs1
VI_VFP_VF_LOOP
({
  vd = f32_sub(vs2, rs1);
})
