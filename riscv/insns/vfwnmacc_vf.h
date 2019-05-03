// vfwnmacc.vf vd, vs2, rs1
VFP_VVF_LOOP_WIDE
({
  switch(p->VU.vsew){
  case e32:
    vd = f64_mulAdd(f64(rs1.v ^ F64_SIGN), vs2, vd);
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
    break;
  };
})
