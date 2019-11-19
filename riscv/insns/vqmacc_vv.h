// vqsmacc.vv vd, vs2, vs1
VI_VV_LOOP_QUAD
({
  VI_QUAD_OP_AND_ASSIGN(vs2, vs1, vd_w, *, +, int);
})
