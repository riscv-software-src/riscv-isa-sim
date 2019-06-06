// vsaddu vd, vs2, zimm5
VI_VI_ULOOP
({
  bool sat = false;
  vd = vs2 + simm5;

  sat = vd < vs2;
  vd |= -(vd < vs2);

  P.VU.vxsat |= sat;
})
