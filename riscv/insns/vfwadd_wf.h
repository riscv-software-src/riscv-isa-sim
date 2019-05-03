// vfwadd.wf vd, vs2, vs1
VFP_VF_LOOP
({
  switch(p->VU.vsew){
  case e32:
    P.VU.elt<float64_t>(rd_num, i) = f64_add(P.VU.elt<float64_t>(rs2_num, i),
                                             f32_to_f64(rs1));
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
  };
})
