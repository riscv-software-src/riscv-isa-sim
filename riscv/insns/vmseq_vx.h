#ifdef BODY
// vseq.vx vd, vs2, rs1
VI_VX_LOOP_CMP
({
  res = rs1 == vs2;
})

#endif