// vfnmsac.vv vd, vs1, vs2, vm   # vd[i] = -(vs2[i] * vs1[i]) + vd[i]
VFP_VV_LOOP
({
 switch(P.VU.vsew){
 case e32:
    vd = f32_mulAdd(f32(vs1.v ^ F32_SIGN), vs2, vd);
    break;
 case e16:
 case e8:
 default:
     softfloat_exceptionFlags = 1;
 };
})
