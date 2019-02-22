require(STATE.VU.vsew == 32);
require(insn.v_vm() == 1);
reg_t vl = STATE.VU.vl;
reg_t baseAddr = RS1;
reg_t vs3 = insn.rd();
for (reg_t i=STATE.VU.vstart; i<vl; ++i){
  MMU.store_uint32(baseAddr + i * 4, STATE.VU.elt<uint32_t>(vs3, i));
}
STATE.VU.vstart = 0;
