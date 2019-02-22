// vlsseg4w.v vd, (rs1), rs2, vm
require(STATE.VU.vsew == 32);
require(insn.v_vm() == 1);
reg_t nf = 4;
reg_t vl = STATE.VU.vl;
reg_t baseAddr = RS1;
reg_t stride = RS2;
reg_t vd = insn.rd();
for (reg_t i=STATE.VU.vstart; i<vl; ++i){
  for (reg_t fn=0; fn<nf; ++fn){
    STATE.VU.elt<uint32_t>(vd+fn, i) = MMU.load_int32(baseAddr + i*stride + fn*4);
  }
}
STATE.VU.vstart = 0;
