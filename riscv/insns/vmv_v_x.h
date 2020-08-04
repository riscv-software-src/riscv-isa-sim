// vmv.v.x vd, rs1
require_vector(true);
VI_CHECK_SSS(false);
VI_VVXI_MERGE_LOOP
({
  vd = rs1;
})
