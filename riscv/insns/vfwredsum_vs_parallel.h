// Parallel version of vfwredsum
VI_CHECK_REDUCTION(false)
VI_VFP_COMMON

switch(p->VU.vsew) {
  case e16: {
    VI_VFP_LOOP_REDUCTIONSUM_WIDEN_INIT(16, 32)
    VI_VFP_LOOP_REDUCTIONSUM_MERGE(32)
    VI_VFP_LOOP_REDUCTIONSUM_CLOSE(e32)
    break;
  }
  case e32: {
    VI_VFP_LOOP_REDUCTIONSUM_WIDEN_INIT(32, 64)
    VI_VFP_LOOP_REDUCTIONSUM_MERGE(64)
    VI_VFP_LOOP_REDUCTIONSUM_CLOSE(e64)
    break;
  }
  default:
    require(0);
    break;
};
