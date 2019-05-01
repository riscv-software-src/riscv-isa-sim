// vfmacc.vv vd, rs1, vs2, vm    # vd[i] = +(vs2[i] * vs1[i]) + vd[i]
VFP_VV_LOOP
({
  switch(p->VU.vsew){
  case e32:
    vd = f32_mulAdd(vs1, vs2, vd);
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
    break;
  };
})
