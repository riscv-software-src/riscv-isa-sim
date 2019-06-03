// vfsgnx
VFP_VF_LOOP
({
  switch(P.VU.vsew){
  case e32:
    vd = fsgnj32(rs1.v, vs2.v, false, true);
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
  };
})
