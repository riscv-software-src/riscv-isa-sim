// vremu.vx vd, vs2, rs1
VI_VX_ULOOP
({
  if (rs1 == 0)
    vd = vs2;
  else if (xlen == 64)
    vd = vs2 % (uint64_t)rs1;
  else
    vd = vs2 % (uint32_t)rs1;
})
