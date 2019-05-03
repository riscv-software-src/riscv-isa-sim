// vfwnmsac.vv vd, vs2, vs1
VFP_VVF_LOOP_WIDE
({
  switch(p->VU.vsew){
  case e32:
    vd = f64_mulAdd(f64(vs1.v ^ F64_SIGN), vs2,
                    f64(vd.v ^ F64_SIGN));
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
    break;
  };
})
