// vfwmsac.vf vd, vs2, rs1
VI_VFP_VF_LOOP_WIDE
({
  vd = f64_mulAdd(rs1, vs2, f64(vd.v ^ F64_SIGN));
})
