// vfdiv.vf vd, vs2, rs1
VI_VFP_VF_LOOP
({
  switch(P.VU.vsew){
  case e32:
    vd = f32_div(vs2, rs1);
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
  };
})
