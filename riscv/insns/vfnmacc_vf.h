// vfnmacc: vd[i] = -(f[rs1] * vs2[i]) - vd[i]
require_fp;
softfloat_roundingMode = STATE.VU.vxrm;
float32_t mul_result;

VFP_VF_LOOP
({
 switch(STATE.VU.vsew){
 case e32:
 mul_result = f32_mul(rs1, f32(vs2.v ^ F32_SIGN));
 vd = f32_sub(mul_result, vd);
 break;
 case e16:
 case e8:
 default:
 softfloat_exceptionFlags = 1;
 };
 })
set_fp_exceptions;
