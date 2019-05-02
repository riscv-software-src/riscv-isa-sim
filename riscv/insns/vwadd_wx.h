// vwaddu.wx vd, vs2, rs1
V_WIDE_CHECK;
VI_VX_LOOP
({
  VI_WIDE_WVX_OP(rs1, +, int);
})
