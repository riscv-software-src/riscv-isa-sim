// VFUNARY0 encoding space

reg_t softfloat_roundingMode = RM;
VFP_VV_LOOP
({
 switch(STATE.VU.vsew){
 case e32:
    if (vs1 == VFUNARY0::VFCVT_XU_F_V){ 
        vd = f32_to_u32(f32(vs2), RM, true);
    }else if(vs1 == VFUNARY0::VFCVT_X_F_V){ 
        vd = f32_to_i32(f32(vs2), RM, true);
    }else if(vs1 == VFUNARY0::VFCVT_F_XU_V){ 
        vd = u32_to_f32(f32(vs2), RM, true);
    }else if(vs1 == VFUNARY0::VFCVT_F_X_V){
        vd = i32_to_f32(f32(vs2), RM, true);
    }else if(vs1 == VFUNARY0::VFWCVT_XU_F_V){
    }else if(vs1 == VFUNARY0::VFWCVT_X_F_V){
    }else if(vs1 == VFUNARY0::VFWCVT_F_XU_V){
    }else if(vs1 == VFUNARY0::VFWCVT_F_X_V){
    }else if(vs1 == VFUNARY0::VFWCVT_F_F_V){
    }else if(vs1 == VFUNARY0::VFNCVT_XU_F_V){
    }else if(vs1 == VFUNARY0::VFNCVT_X_F_V){
    }else if(vs1 == VFUNARY0::VFNCVT_F_XU_V){
    }else if(vs1 == VFUNARY0::VFNCVT_F_X_V){
    }else if(vs1 == VFUNARY0::VFNCVT_F_F_V){
    }
    break;
 case e16:
 case e8:
 default:
     softfloat_exceptionFlags = 1;
 };
})
