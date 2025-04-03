// vsaddminu vd, vs2, rs1

VI_VX_ULOOP
({
  bool sat = false;
  bool addmin_order = RS1>>63; // 0 for add-min, 1 for min-add
  reg_t tmp;
  if(addmin_order){
    // min-sadd
    if(vd<=vs2){
      tmp = vd + rs1;

      sat = tmp < vd;
      tmp |= -(sat);
      vd = tmp;
    } else {
      vd = vs2 + rs1;
      sat = vd < vs2;
      vd |= -(vd < vs2);
    }
  } else {
    // sadd-min
    tmp = vs2 + rs1;
    sat = tmp < vs2;
    tmp |= -(sat);

    if(vd>tmp){
      vd = tmp;
    }
  }

  P_SET_OV(sat);

})
