// vfadd.vf vd, vs2, rs1
VI_VFP_VF_LOOP2
({
    vd = f32_add(rs1, vs2);
})
