// vnsra.vv vd, vs2, vs1
VI_VVXI_LOOP_NARROW
({
  vd = vs2 >> (vs1 & ((1u << log2(sew)) - 1));
})
