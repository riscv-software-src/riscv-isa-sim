// vmv1r.v vd, vs2
require_vector_novtype(true, true);
const reg_t baseAddr = RS1;
const reg_t vd = insn.rd();
const reg_t vs2 = insn.rs2();
const reg_t len = insn.rs1() + 1;
require_align(vd, len);
require_align(vs2, len);
const reg_t size = len * p->VU.vlenb;

//register needs one-by-one copy to keep commitlog correct
if (vd != vs2 && p->VU.vstart->read() < size) {
  reg_t i = p->VU.vstart->read() / p->VU.vlenb;
  reg_t off = p->VU.vstart->read() % p->VU.vlenb;
  if (off) {
    memcpy(&p->VU.elt<uint8_t>(vd + i, off, true),
           &p->VU.elt<uint8_t>(vs2 + i, off), p->VU.vlenb - off);
    i++;
  }

  for (; i < len; ++i) {
    memcpy(&p->VU.elt<uint8_t>(vd + i, 0, true),
           &p->VU.elt<uint8_t>(vs2 + i, 0), p->VU.vlenb);
  }
}

p->VU.vstart->write(0);
