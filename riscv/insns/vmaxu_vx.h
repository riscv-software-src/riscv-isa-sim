// vmaxu.vx vd, vs2, rs1, vm   # vector-scalar
VI_VX_ULOOP
({
  if (rs1 >= vs2) {
    vd = rs1;
  } else {
    vd = vs2;
  }
})
