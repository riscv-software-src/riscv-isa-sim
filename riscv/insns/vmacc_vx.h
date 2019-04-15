// vmacc.vx: vd[i] = +(x[rs1] * vs2[i]) + vd[i]
VI_VX_LOOP
({
  V_LOOP_ELEMENT_SKIP;

  vd = rs1 * vs2 + vd;
})
