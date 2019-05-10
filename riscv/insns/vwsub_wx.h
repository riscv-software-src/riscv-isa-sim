// vwsub.wx vd, vs2, rs1
VI_WIDE_CHECK_TWO;
VI_VX_LOOP
({
  VI_WIDE_WVX_OP(rs1, -, int);
})
