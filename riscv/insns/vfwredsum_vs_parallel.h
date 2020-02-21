// Parallel version of vfwredsum
require_vector;
require(P.VU.vsew * 2 <= P.VU.ELEN);
require((insn.rs2() & (P.VU.vlmul - 1)) == 0);

VI_VFP_COMMON

VI_VFP_LOOP_REDUCTIONSUM_WIDEN_INIT
VI_VFP_LOOP_REDUCTIONSUM_MERGE(64)
VI_VFP_LOOP_REDUCTIONSUM_CLOSE(e64)
