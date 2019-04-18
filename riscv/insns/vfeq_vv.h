// vfeq.vv vd, vs2, vs1
VFP_LOOP_CMP
({
  res = f32_eq(vs2, vs1);
})
