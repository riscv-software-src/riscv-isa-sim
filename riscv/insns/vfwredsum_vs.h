// vfwredsum.vs vd, vs2, vs1
require_vector;
require(P.VU.vsew * 2 <= P.VU.ELEN);
require((insn.rs2() & (P.VU.vlmul - 1)) == 0);
VI_VFP_VV_LOOP_WIDE_REDUCTION
({
  vd_0 = f64_add(vd_0, vs2);
})
