// vmsgtu.vi  vd, vd2, simm5
VI_VI_ULOOP_CMP
({
  res = vs2 > (insn.v_simm5() & (UINT64_MAX >> (64 - p->VU.vsew)));
})
