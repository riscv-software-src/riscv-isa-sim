// vfwnmacc.vf vd, vs2, rs1
VFP_VVF_LOOP_WIDE
({
  switch(P.VU.vsew){
  case e32:
    vd = f64_mulAdd(f64(rs1.v ^ F64_SIGN), vs2,
                    f64(vd.v ^ F64_SIGN));
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
    break;
  };
})
VI_CHECK_1905
