// vfadd.vf vd, vs2, rs1
VFP_VF_LOOP
({
 switch(p->VU.vsew){
 case e32:
    vd = f32_add(rs1, vs2);
    break;
 case e16:
 case e8:
 default:
     softfloat_exceptionFlags = 1;
 };
})
