// vadc.vv vd, vs2, rs1
require(insn.v_vm() == 1);
require(!(insn.rd() == 0 && p->VU.vlmul > 1));
VI_VV_LOOP
({
  auto &v0 = P.VU.elt<uint64_t>(0, midx);
  const uint64_t mmask = (UINT64_MAX << (64 - mlen)) >> (64 - mlen - mpos);
  const uint128_t op_mask = (UINT64_MAX >> (64 - sew));
  uint64_t carry = (v0 >> mpos) & 0x1;

  uint128_t res = (op_mask & vs1) + (op_mask & vs2) + carry;
  vd = res;

  carry = (res >> sew) & 0x1u;
  v0 = (v0 & ~mmask) | ((carry << mpos) & mmask);
})

VI_TAIL_ZERO_MASK(0);
VI_CHECK_1905
