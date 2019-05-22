// vsaddu vd, vs2, zimm5
VI_VI_ULOOP
({
  bool sat = false;
  vd = vs2 + vsext(simm5, sew);
  
  sat = vd < vs2;
  vd |= -(vd < vs2);

  p->VU.vxsat |= sat;
})
VI_CHECK_1905
