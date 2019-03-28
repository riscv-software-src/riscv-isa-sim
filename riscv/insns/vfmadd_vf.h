// vfmadd: vd[i] = +(vd[i] * f[rs1]) + vs2[i]
require_fp;
softfloat_roundingMode = STATE.VU.vxrm;

VFP_VF_LOOP
({
 switch(STATE.VU.vsew){
 case e32:
    vd = f32_mulAdd(vd, rs1, vs2);
    break;
 case e16:
 case e8:
 default:
     softfloat_exceptionFlags = 1;
 };
})
set_fp_exceptions;
