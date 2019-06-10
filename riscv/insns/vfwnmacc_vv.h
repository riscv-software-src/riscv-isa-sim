// vfwnmacc.vv vd, vs2, vs1
VI_VFP_VV_LOOP_WIDE
({
  switch(P.VU.vsew) {
  case e32:
    vd = f64_mulAdd(f64(vs1.v ^ F64_SIGN), vs2, f64(vd.v ^ F64_SIGN));
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
    break;
  };
})
VI_CHECK_1905
