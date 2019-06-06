// vfsgnj vd, vs2, vs1
VI_VFP_VF_LOOP
({
  vd = fsgnj32(rs1.v, vs2.v, false, false);
})
