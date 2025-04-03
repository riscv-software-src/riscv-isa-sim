// vsaddminu vd, vs2, vs1

VI_VV_ULOOP
({
    bool sat = false;
    // sadd-min
    reg_t sum;
    sum = vs2 + vs1;
    sat = sum < vs2;
    sum |= -(sat);

    if(vd>sum){
      vd = sum;
    }

    P_SET_OV(sat);
})
