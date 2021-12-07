// vfmerge_vf vd, vs2, vs1, vm
VI_CHECK_SSS(false);
VI_VFP_COMMON;

switch(P.VU.vsew) {
  case e16:
    for (reg_t i=P.VU.vstart->read(); i<vl; ++i) {
      VFP_VF_PARAMS(16);

      int midx = i / 64;
      int mpos = i % 64;
      bool use_first = (P.VU.elt<uint64_t>(0, midx) >> mpos) & 0x1;

      vd = use_first ? rs1 : vs2;
    }
    break;
  case e32:
    for (reg_t i=P.VU.vstart->read(); i<vl; ++i) {
      VFP_VF_PARAMS(32);

      int midx = i / 64;
      int mpos = i % 64;
      bool use_first = (P.VU.elt<uint64_t>(0, midx) >> mpos) & 0x1;

      vd = use_first ? rs1 : vs2;
    }
    break;
  case e64:
    for (reg_t i=P.VU.vstart->read(); i<vl; ++i) {
      VFP_VF_PARAMS(64);

      int midx = i / 64;
      int mpos = i % 64;
      bool use_first = (P.VU.elt<uint64_t>(0, midx) >> mpos) & 0x1;

      vd = use_first ? rs1 : vs2;
    }
    break;
  default:
    require(0);
    break;
}

P.VU.vstart->write(0);
