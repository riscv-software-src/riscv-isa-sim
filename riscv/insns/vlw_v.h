require(STATE.VU.vsew == 32);
require(insn.v_vm() == 1);
reg_t vl = STATE.VU.vl;
reg_t baseAddr = RS1;
reg_t vd = insn.rd();
for (reg_t i=STATE.VU.vstart; i<vl; ++i){
  STATE.VU.elt<uint32_t>(vd, i) = MMU.load_int32(baseAddr + i * 4);
}
STATE.VU.vstart = 0;
