require(STATE.VU.vsew == 32 && insn.v_nf() == 0);
reg_t vl = STATE.VU.vl;
reg_t baseAddr = RS1;
reg_t srcVReg = insn.rd();
for (reg_t i=STATE.VU.vstart; i<vl; ++i){
  MMU.store_uint32(baseAddr + i * 4, STATE.VU.elt<uint32_t>(srcVReg, i));
  #if 0
  printf("Stored addr %16p with %f\n",
         &STATE.VU.elt<uint32_t>(srcVReg, i),
         STATE.VU.elt<float>(srcVReg, i));
  #endif
}
STATE.VU.vstart = 0;
