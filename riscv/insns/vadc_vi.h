// vadc.vi vd, vs2, simm5
require(insn.v_vm() == 1);
VI_VI_LOOP
({
  uint32_t &v0 = P.VU.elt<uint32_t>(0, midx);
  int carry = (v0 >> mpos) & 0x1;

  unsigned __int128 res = simm5 + vs2 + carry;
  vd = res;

  const uint32_t mmask = ((1ul << mlen) - 1) << mpos;
  carry = (res >> sew) & 0x1u;
  v0 = (v0 & ~mmask) | ((carry << mpos) & mmask);
})
