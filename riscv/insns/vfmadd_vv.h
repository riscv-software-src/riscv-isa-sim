// vfmadd: vd[i] = +(vd[i] * vs1[i]) + vs2[i]
require_fp;
softfloat_roundingMode = STATE.VU.vxrm;

VFP_VV_LOOP
({
 switch(STATE.VU.vsew){
 case e32:
    vd = f32_mulAdd(vd, vs1, vs2);
    break;
 case e16:
 case e8:
 default:
     softfloat_exceptionFlags = 1;
 };
})
set_fp_exceptions;
