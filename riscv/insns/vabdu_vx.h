// vabdu.vx vd, vs2, rs1, vm

require_zvabd;

VI_VX_ULOOP
({
  vd = DO_ABDU(vs2, rs1);
})
