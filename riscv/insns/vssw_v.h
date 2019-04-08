// vssw.v and vssseg[2-8]w.v
require(STATE.VU.vsew >= e32);
reg_t nf = insn.v_nf() + 1;
require((nf >= 2 && STATE.VU.vlmul == 1) || nf == 1);
reg_t vl = STATE.VU.vl;
reg_t baseAddr = RS1;
reg_t stride = RS2;
reg_t vs3 = insn.rd();
for (reg_t i = STATE.VU.vstart; i < vl; ++i) {
  V_LOOP_ELEMENT_SKIP;

  for (reg_t fn = 0; fn < nf; ++fn) {
    uint32_t val = 0;
    switch (STATE.VU.vsew) {
    case e32:
      val = STATE.VU.elt<uint32_t>(vs3 + fn, i);
      break;
    defaualt:
      val = STATE.VU.elt<uint64_t>(vs3 + fn, i);
      break;
    }
    MMU.store_uint32(baseAddr + i * stride + fn * 4, val);
  }
}
STATE.VU.vstart = 0;
