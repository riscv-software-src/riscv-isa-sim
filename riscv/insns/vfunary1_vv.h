// VFUNARY1 encoding space
VFP_VV_LOOP
({
 switch(p->VU.vsew){
 case e32:
    if (RS1 == VFUNARY1::VFSQRT_V){
        vd = f32_sqrt(vs2);
    }else if (RS1 == VFUNARY1::VFCLASS_V){
        class_vd = f32_classify(vs2);
    }
    break;
 case e16:
 case e8:
    assert(RS1 != VFUNARY1::VFCLASS_V);
 default:
     softfloat_exceptionFlags = 1;
 };
})
