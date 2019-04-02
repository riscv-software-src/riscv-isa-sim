// vfredmin
bool less;
VFP_VV_REDUCTION_LOOP
({
    less = f32_lt_quiet(vd_0, vs2) || (f32_eq(vd_0, vs2) && (vd_0.v & F32_SIGN));

    if(isNaNF32UI(vd_0.v) && isNaNF32UI(vs2.v)){
      vd_0 = f32(defaultNaNF32UI);
    }else{
      vd_0 = (less || isNaNF32UI(vs2.v) ? vd_0 : vs2);
    }

 })
