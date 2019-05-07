// vnsrl.vv vd, vs2, vs1
VI_VVXI_LOOP_NARROW
({
  vd = vs2_u >> (vs1 & ((1u << log2(sew)) - 1));
})
