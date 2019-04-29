// vmsac: vd[i] = -(x[rs1] * vs2[i]) + vd[i]
VI_VX_LOOP
({
  vd = -(rs1 * vs2) + vd;
})
