// vfmsub: vd[i] = +(vd[i] * f[rs1]) - vs2[i]
require_fp;
softfloat_roundingMode = STATE.VU.vxrm;
float32_t mul_result;

VFP_VF_LOOP
({
 switch(STATE.VU.vsew){
 case e32:
    mul_result = f32_mul(vd, rs1);
    vd = f32_sub(mul_result, vs2);
    break;
 case e16:
 case e8:
 default:
     softfloat_exceptionFlags = 1;
 };
})
set_fp_exceptions;
