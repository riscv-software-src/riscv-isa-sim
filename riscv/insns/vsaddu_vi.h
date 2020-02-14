// vsaddu vd, vs2, zimm5
VI_VI_ULOOP
({
  bool sat = false;
  uint64_t imm = (int8_t)(zimm5 << 3) >> 3;
  vd = vs2 + (imm & (UINT64_MAX >> (64 - P.VU.vsew)));

  sat = vd < vs2;
  vd |= -(vd < vs2);

  P.VU.vxsat |= sat;
})
