// vmv1r.v vd, vs2
require_vector_novtype(true);
const reg_t baseAddr = RS1;
const reg_t vd = insn.rd();
const reg_t vs2 = insn.rs2();
const reg_t len = insn.rs1() + 1;
require((vd & (len - 1)) == 0);
require((vs2 & (len - 1)) == 0);
const reg_t size = len * P.VU.vlenb;
if (vd != vs2 && P.VU.vstart < size) {
  memcpy(&P.VU.elt<uint8_t>(vd, P.VU.vstart, true),
         &P.VU.elt<uint8_t>(vs2, P.VU.vstart), size - P.VU.vstart);
}
P.VU.vstart = 0;
