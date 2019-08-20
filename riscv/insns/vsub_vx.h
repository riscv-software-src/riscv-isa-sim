// vsub: vd[i] = (vd[i] * x[rs1]) - vs2[i]
VI_VX_LOOP
({
  vd = vs2 - rs1;
})
