// vfsgnx
softfloat_roundingMode = STATE.VU.vxrm;
VFP_VV_LOOP
({
 switch(STATE.VU.vsew){
 case e32:
    //fsgnj32(vs1, vs2, false, true));
    vd = f32((vs1.v & ~F32_SIGN) | ((((true) ? vs2.v : (false) ? F32_SIGN : 0) ^ vs2.v) & F32_SIGN));
    break;
 case e16:
 case e8:
 default:
     softfloat_exceptionFlags = 1;
 };
})
set_fp_exceptions;
