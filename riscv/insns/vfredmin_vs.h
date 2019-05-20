// vfredmin vd, vs2, vs1
VFP_VV_LOOP_REDUCTION
({
    vd_0 = f32_min(vd_0, vs2);
})
VI_CHECK_1905
