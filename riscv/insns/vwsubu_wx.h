// vwsubu.wx vd, vs2, rs1
VI_CHECK_DDS(false);
VI_VX_LOOP_WIDEN
({
  VI_WIDE_WVX_OP(rs1, -, uint);
})
