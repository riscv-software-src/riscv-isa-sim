// vadc.vv vd, vs2, rs1
require(insn.v_vm() == 1);
VI_VV_LOOP
({
  uint32_t &v0 = P.VU.elt<uint32_t>(0, midx);
  int carry = (v0 >> mpos) & 0x1;

  uint128_t res = vs1 + vs2 + carry;
  vd = res;

  const uint32_t mmask = ((1ul << mlen) - 1) << mpos;
  carry = (res >> sew) & 0x1u;
  v0 = (v0 & ~mmask) | ((carry << mpos) & mmask);
})
VI_CHECK_1905
