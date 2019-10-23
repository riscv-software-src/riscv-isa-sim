// vvmv.v.v vd, vs1
require((insn.rs1() & (P.VU.vlmul - 1)) == 0);
VI_VVXI_MERGE_LOOP
({
  vd = vs1;
})
