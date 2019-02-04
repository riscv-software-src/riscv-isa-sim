reg_t vl = STATE.VU.vl;
reg_t baseAddr = RS1 + insn.v_off() * 4;
reg_t dstVReg = insn.rd();
for (reg_t i=STATE.VU.vstart; i<vl; ++i){
  STATE.VU.elt<uint32_t>(dstVReg, i) = MMU.load_int32(baseAddr + i * 4);
  #if 0
  printf("Loaded addr %16p with %f\n",
         &STATE.VU.elt<uint32_t>(dstVReg, i),
         STATE.VU.elt<float>(dstVReg, i));
  #endif
}
STATE.VU.vstart = 0;
