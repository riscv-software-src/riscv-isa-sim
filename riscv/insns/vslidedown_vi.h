// vslidedown.vi vd, vs2, rs1
VI_CHECK_SLIDE(false);

const reg_t sh = insn.v_zimm5();
//VI_LOOP_BASE
\
require(P.VU.vsew >= e8 && P.VU.vsew <= e64); \
require_vector(true); \
reg_t vl = P.VU.vl->read(); \
reg_t UNUSED sew = P.VU.vsew; \
reg_t rd_num = insn.rd(); \
reg_t UNUSED rs1_num = insn.rs1(); \
reg_t rs2_num = insn.rs2(); \
V_EXT_VSTART_CHECK; \
for (reg_t i = P.VU.vstart->read(); i < std::max(P.VU.vlmax, P.VU.VLEN/P.VU.vsew); ++i) { \
\
VI_MASK_VARS \
__attribute__((unused)) int mata_action = 0; \
bool skip = false; \
mata_action = 0; \
if (insn.v_vm() == 0) { \
  skip = ((P.VU.elt<uint64_t>(0, midx) >> mpos) & 0x1) == 0; \
  if (skip && 0 == P.VU.vma && i < vl) { \
      continue; \
  } \
} \
\

reg_t offset = 0;
bool is_valid = (i + sh) < P.VU.vlmax;

if (is_valid) {
  offset = sh;
}
if (0 == P.VU.vta && i >= vl) { \
  continue; \
} \
if ((true == skip && 1 == P.VU.vma && i < vl) || (1 == P.VU.vta && i >= vl)) \
  mata_action = 2; \
else \
  mata_action = 1; \
switch (sew) {
case e8: {
  VI_XI_SLIDEDOWN_PARAMS(e8, offset);
  if (1 == mata_action) \
    vd = is_valid ? vs2 : 0; \
  else \
    vd = 0xFF; \
}
break;
case e16: {
  VI_XI_SLIDEDOWN_PARAMS(e16, offset);
  if (1 == mata_action) \
    vd = is_valid ? vs2 : 0; \
  else \
    vd = 0xFFFF; \
}
break;
case e32: {
  VI_XI_SLIDEDOWN_PARAMS(e32, offset);
  if (1 == mata_action) \
    vd = is_valid ? vs2 : 0; \
  else \
    vd = 0xFFFFFFFF; \
}
break;
default: {
  VI_XI_SLIDEDOWN_PARAMS(e64, offset);
  if (1 == mata_action) \
    vd = is_valid ? vs2 : 0; \
  else \
    vd = 0xFFFFFFFFFFFFFFFF; \
}
break;
}
VI_LOOP_END
