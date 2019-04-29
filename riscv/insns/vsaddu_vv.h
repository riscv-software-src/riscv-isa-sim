// vsaddu vd, vs2, vs1
VI_VV_ULOOP
({
  bool sat = false;
  vd = vs2 + vs1;

  sat = vd < vs2;
  vd |= -(vd < vs2);

  STATE.VU.vxsat |= sat;
})
