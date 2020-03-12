// vmv1r.v vd, vs2
require_vector;
const reg_t baseAddr = RS1;
const reg_t vd = insn.rd();
const reg_t vs2 = insn.rs2();
const reg_t len = insn.rs1() + 1;
require((vd & (len - 1)) == 0);
require((vs2 & (len - 1)) == 0);
if (vd != vs2) {
  for (reg_t i = 0; i < len; ++i) {
    memcpy(&P.VU.elt<uint8_t>(vd + i, 0, true),
           &P.VU.elt<uint8_t>(vs2 + i, 0), P.VU.vlenb);
  }
}
P.VU.vstart = 0;
