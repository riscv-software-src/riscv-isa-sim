// vmsub.vx: vd[i] = (vd[i] * x[rs1]) - vs2[i]
VI_VX_LOOP
({
  V_LOOP_ELEMENT_SKIP;

  vd = (vd * rs1) - vs2;
})
