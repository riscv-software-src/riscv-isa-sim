// VFUNARY1 encoding space
VI_VFP_VV_LOOP
({
  switch(P.VU.vsew){
  case e32:
    if (rs1_num == VFUNARY1::VFSQRT_V) {
      vd = f32_sqrt(vs2);
    } else if (rs1_num == VFUNARY1::VFCLASS_V) {
      vd.v = f32_classify(vs2);
    }
    break;
  case e16:
  case e8:
    assert(RS1 != VFUNARY1::VFCLASS_V);
  default:
    softfloat_exceptionFlags = 1;
  };
})
