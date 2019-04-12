// vsaddu vd, vs2, rs1
VI_VX_ULOOP
({
  V_LOOP_ELEMENT_SKIP;

  bool sat = false;
  vd = vs2 + rs1;

  sat = vd < vs2;
  vd |= -(vd < vs2);

  STATE.VU.vxsat |= sat;

})
