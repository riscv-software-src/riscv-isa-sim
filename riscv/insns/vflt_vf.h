// vflt
require_fp;
softfloat_roundingMode = STATE.VU.vxrm;
VFP_VV_LOOP
({
 bool &vd = STATE.VU.elt<bool>(rd_num, i); \
 switch(STATE.VU.vsew){
 case e32:
    vd = f32_lt(f32(rs1), vs2);
    break;
 case e16:
 case e8:
 default:
     softfloat_exceptionFlags = 1;
 };
})
set_fp_exceptions;
