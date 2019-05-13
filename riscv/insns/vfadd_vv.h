// vfadd.vv vd, vs2, vs1
VFP_VV_LOOP
({
 switch(p->VU.vsew){
 case e32:
    vd = f32_add(vs1, vs2);
    break;
 case e16:
 case e8:
 default:
     softfloat_exceptionFlags = 1;
 };
})
