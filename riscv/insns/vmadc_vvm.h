// vmadc.vvm vd, vs2, rs1
VI_VV_LOOP_CARRY
({
  auto v0 = P.VU.elt<uint64_t>(0, midx);
  const uint64_t mmask = UINT64_C(1) << mpos; \
  const uint128_t op_mask = (UINT64_MAX >> (64 - sew));
  uint64_t carry = insn.v_vm() == 0 ? (v0 >> mpos) & 0x1 : 0;

  uint128_t res = (op_mask & vs1) + (op_mask & vs2) + carry;

  carry = (res >> sew) & 0x1u;
  vd = (vd & ~mmask) | ((carry << mpos) & mmask);
})
