// vmulhsu.vx vd, vs2, rs1
VI_VX_LOOP
({
  vd = ((int128_t)vs2 * (uint128_t)rs1) >> sew;
})
