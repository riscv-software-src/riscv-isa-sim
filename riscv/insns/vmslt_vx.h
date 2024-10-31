#ifdef BODY
// vslt.vx  vd, vs2, vs1
VI_VX_LOOP_CMP
({
  res = vs2 < rs1;
})

#endif