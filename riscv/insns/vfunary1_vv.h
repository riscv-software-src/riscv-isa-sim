// VFUNARY1 encoding space
VI_VFP_VV_LOOP
({
    if (rs1_num == VFUNARY1::VFSQRT_V) {
      vd = f32_sqrt(vs2);
    } else if (rs1_num == VFUNARY1::VFCLASS_V) {
      vd.v = f32_classify(vs2);
    }
})
