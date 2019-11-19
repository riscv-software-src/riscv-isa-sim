// vqmaccsu.vx vd, vs2, rs1
VI_VV_LOOP_QUAD
({
  VI_QUAD_OP_AND_ASSIGN_MIX(vs2, vs1, vd_w, *, +, int, uint, int);
})
