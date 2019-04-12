// vsra.vx vd, vs2, rs1
VI_VX_LOOP
({
  V_LOOP_ELEMENT_SKIP;

  vd = vs2 >> (rs1 & ((1u << log2(sew)) - 1));
})
