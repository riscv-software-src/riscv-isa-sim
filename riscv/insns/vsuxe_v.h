// vsxe.v and vsxseg[2-8]e.v
const reg_t sew = p->VU.vsew;
const reg_t vl = p->VU.vl;
require(sew >= e8 && sew <= e64);
reg_t baseAddr = RS1;
reg_t stride = insn.rs2();
reg_t vs3 = insn.rd();
for (reg_t i = p->VU.vstart; i < vl; ++i) {
  V_LOOP_ELEMENT_SKIP;

  reg_t index = 0;
  switch (sew) {
  case e8:
    index = p->VU.elt<int8_t>(stride, i);
    MMU.store_uint8(baseAddr + index,
                    p->VU.elt<uint8_t>(vs3, i));
    break;
  case e16:
    index = p->VU.elt<int16_t>(stride, i);
    MMU.store_uint16(baseAddr + index,
                     p->VU.elt<uint16_t>(vs3, i));
    break;
  case e32:
    index = p->VU.elt<int32_t>(stride, i);
    MMU.store_uint32(baseAddr + index,
                     p->VU.elt<uint32_t>(vs3, i));
    break;
  case e64:
    index = p->VU.elt<int64_t>(stride, i);
    MMU.store_uint64(baseAddr + index,
                     p->VU.elt<uint64_t>(vs3, i));
    break;
  }
}
p->VU.vstart = 0;
VI_CHECK_1905
