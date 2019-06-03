// vfsgnx
VFP_VV_LOOP
({
  switch(P.VU.vsew){
  case e32:
    vd = fsgnj32(vs1.v, vs2.v, false, true);
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
  };
})
