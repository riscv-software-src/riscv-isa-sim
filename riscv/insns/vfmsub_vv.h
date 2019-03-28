// vfmsub: vd[i] = +(vd[i] * vs1[i]) - vs2[i]

require_fp;
softfloat_roundingMode = STATE.VU.vxrm;
float32_t mul_result;

VFP_VV_LOOP
({
 switch(STATE.VU.vsew){
 case e32:
    mul_result = f32_mul(vd, vs1);
    vd = f32_sub(mul_result, vs2);
    break;
 case e16:
 case e8:
 default:
     softfloat_exceptionFlags = 1;
 };
})
set_fp_exceptions;
