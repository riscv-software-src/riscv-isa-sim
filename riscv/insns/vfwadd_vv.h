// vfwadd.vv vd, vs2, vs1
VI_VFP_VV_LOOP_WIDE
({
  switch(P.VU.vsew) {
  case e32:
    vd = f64_add(vs2, vs1);
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
  };
})
VI_CHECK_1905
