// vfeq.vv vd, vs2, vs1
VI_VFP_LOOP_CMP
({
  res = f32_eq(vs2, vs1);
})
