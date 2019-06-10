// vfwsub.wf vd, vs2, rs1
VI_VFP_WF_LOOP_WIDE
({
  switch(P.VU.vsew) {
  case e32:
    vd = f64_sub(vs2, rs1);
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
  };
})
VI_CHECK_1905
