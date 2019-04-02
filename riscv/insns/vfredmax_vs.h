// vfredmax
bool greater;
VFP_VV_REDUCTION_LOOP
({
    greater = f32_lt_quiet(vs2, vd_0) || (f32_eq(vs2, vd_0) && (vs2.v & F32_SIGN));

    if(isNaNF32UI(vd_0.v) && isNaNF32UI(vs2.v)){
      vd_0 = f32(defaultNaNF32UI);
    }else{
      vd_0 = (greater || isNaNF32UI(vs2.v) ? vd_0 : vs2);
    }

})
