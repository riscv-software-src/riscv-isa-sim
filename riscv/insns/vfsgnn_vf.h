// vfsgnn
softfloat_roundingMode = STATE.VU.vxrm;
VFP_VF_LOOP
({
 switch(STATE.VU.vsew){
 case e32:
    //fsgnj32(rs1, vs2, true, false));
    vd = f32((rs1.v & ~F32_SIGN) | ((((false) ? vs2.v : (true) ? F32_SIGN : 0) ^ vs2.v) & F32_SIGN));
    break;
 case e16:
 case e8:
 default:
     softfloat_exceptionFlags = 1;
 };
})
set_fp_exceptions;
