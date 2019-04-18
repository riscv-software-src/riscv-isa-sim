// vfdiv.vf vd, vs2, rs1
VFP_VF_LOOP
({
 switch(STATE.VU.vsew){
 case e32:
    vd = f32_div(vs2, rs1);
    break;
 case e16:
 case e8:
 default:
     softfloat_exceptionFlags = 1;
 };
})
set_fp_exceptions;
