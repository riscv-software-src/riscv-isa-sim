// vsrl
VI_VX_LOOP
({
    vd = vzext(vs2 >> (rs1 & (sew - 1)), sew);
})
