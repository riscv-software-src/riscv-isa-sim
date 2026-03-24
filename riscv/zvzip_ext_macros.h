#ifndef RISCV_ZVZIP_MACROS_H_
#define RISCV_ZVZIP_MACROS_H_

#define require_zvzip \
  do { \
    require_vector(true); \
    require_extension(EXT_ZVZIP); \
  } while (0)

#define ZVZIP_EXTRACT_V(x, VS2_IDX) \
  type_sew_t<x>::type UNUSED &vd = P.VU.elt<type_sew_t<x>::type>(rd_num, i, true); \
  vd = P.VU.elt<type_sew_t<x>::type>(rs2_num, VS2_IDX);

#define ZVZIP_EXTRACT_VV(x, VS1_IDX, VS2_IDX) \
  type_sew_t<x>::type UNUSED &vd = P.VU.elt<type_sew_t<x>::type>(rd_num, i, true); \
  vd = (i & 1) \
    ? ((VS1_IDX) >= P.VU.vlmax ? 0 : P.VU.elt<type_sew_t<x>::type>(rs1_num, VS1_IDX)) \
    : ((VS2_IDX) >= P.VU.vlmax ? 0 : P.VU.elt<type_sew_t<x>::type>(rs2_num, VS2_IDX));

#define VI_VZIP_VV_CHECK \
  require_vector(true); \
  require(P.VU.vflmul <= 4); \
  require_align(insn.rd(), P.VU.vflmul * 2); \
  require_align(insn.rs2(), P.VU.vflmul); \
  require_align(insn.rs1(), P.VU.vflmul); \
  require_vm; \
  if (P.VU.vflmul < 1) { \
    require_noover(insn.rd(), P.VU.vflmul * 2, insn.rs2(), P.VU.vflmul); \
    require_noover(insn.rd(), P.VU.vflmul * 2, insn.rs1(), P.VU.vflmul); \
  } else { \
    require_noover_widen(insn.rd(), P.VU.vflmul * 2, insn.rs2(), P.VU.vflmul); \
    require_noover_widen(insn.rd(), P.VU.vflmul * 2, insn.rs1(), P.VU.vflmul); \
  }

#define VI_VZIP_VV_LOOP_BASE \
  require(P.VU.vsew >= e8 && P.VU.vsew <= e64); \
  reg_t vl = P.VU.vl->read() * 2; \
  reg_t UNUSED sew = P.VU.vsew; \
  reg_t UNUSED rd_num = insn.rd(); \
  reg_t UNUSED rs1_num = insn.rs1(); \
  reg_t rs2_num = insn.rs2(); \
  for (reg_t i = P.VU.vstart->read(); i < vl; ++i) { \
    VI_LOOP_ELEMENT_SKIP();

#define VI_VZIP_VV_LOOP_END \
  } \
  P.VU.vstart->write(0);

#define VI_VZIP_VV_LOOP(VS1_IDX, VS2_IDX) \
  VI_VZIP_VV_CHECK \
  VI_VZIP_VV_LOOP_BASE \
  if (sew == e8) { \
    ZVZIP_EXTRACT_VV(e8, VS1_IDX, VS2_IDX); \
  } else if (sew == e16) { \
    ZVZIP_EXTRACT_VV(e16, VS1_IDX, VS2_IDX); \
  } else if (sew == e32) { \
    ZVZIP_EXTRACT_VV(e32, VS1_IDX, VS2_IDX); \
  } else if (sew == e64) { \
    ZVZIP_EXTRACT_VV(e64, VS1_IDX, VS2_IDX); \
  } \
  VI_VZIP_VV_LOOP_END

#define VI_VUNZIP_V_CHECK \
  require_vector(true); \
  require(P.VU.vflmul <= 4); \
  require_align(insn.rs2(), P.VU.vflmul * 2); \
  require_align(insn.rd(), P.VU.vflmul); \
  require_vm; \
  if (insn.rd() != insn.rs2()) { \
    require_noover(insn.rd(), P.VU.vflmul, insn.rs2(), P.VU.vflmul * 2); \
  }

#define VI_VUNZIP_V_LOOP(VS2_IDX) \
  VI_VUNZIP_V_CHECK \
  VI_LOOP_BASE \
  if (sew == e8) { \
    ZVZIP_EXTRACT_V(e8, VS2_IDX); \
  } else if (sew == e16) { \
    ZVZIP_EXTRACT_V(e16, VS2_IDX); \
  } else if (sew == e32) { \
    ZVZIP_EXTRACT_V(e32, VS2_IDX); \
  } else if (sew == e64) { \
    ZVZIP_EXTRACT_V(e64, VS2_IDX); \
  } \
  VI_LOOP_END

#define VI_VPAIR_VV_CHECK \
  require_vector(true); \
  require_align(insn.rd(), P.VU.vflmul); \
  require_align(insn.rs2(), P.VU.vflmul); \
  require_align(insn.rs1(), P.VU.vflmul); \
  require_vm; \
  require_noover(insn.rd(), P.VU.vflmul, insn.rs2(), P.VU.vflmul); \
  require_noover(insn.rd(), P.VU.vflmul, insn.rs1(), P.VU.vflmul);

#define VI_VPAIR_VV_LOOP(VS1_IDX, VS2_IDX) \
  VI_VPAIR_VV_CHECK \
  VI_LOOP_BASE \
  if (sew == e8) { \
    ZVZIP_EXTRACT_VV(e8, VS1_IDX, VS2_IDX); \
  } else if (sew == e16) { \
    ZVZIP_EXTRACT_VV(e16, VS1_IDX, VS2_IDX); \
  } else if (sew == e32) { \
    ZVZIP_EXTRACT_VV(e32, VS1_IDX, VS2_IDX); \
  } else if (sew == e64) { \
    ZVZIP_EXTRACT_VV(e64, VS1_IDX, VS2_IDX); \
  } \
  VI_LOOP_END

#endif
