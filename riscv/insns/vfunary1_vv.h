// VFUNARY1 encoding space

reg_t softfloat_roundingMode = RM;
VFP_VV_LOOP
({
 switch(STATE.VU.vsew){
 case e32:
    if (vs1 == VFUNARY1::VFSQRT_V){ 
        vd = f32_sqrt(f32(vs2));
    }else if (vs1 == VFUNARY1::VFCLASS_V){
        vd = f32_classify(f32(vs2)));
    }
    break;
 case e16:
 case e8:
    assert(vs1 != VFUNARY1::VFCLASS_V);
 default:
     softfloat_exceptionFlags = 1;
 };
})
