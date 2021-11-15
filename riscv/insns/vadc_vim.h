// vadc.vim vd, vs2, simm5, v0
VI_XI_LOOP_WITH_CARRY
({
  auto &v0 = p->VU.elt<uint64_t>(0, midx);
  const uint128_t op_mask = (UINT64_MAX >> (64 - sew));
  uint64_t carry = (v0 >> mpos) & 0x1;

  uint128_t res = (op_mask & simm5) + (op_mask & vs2) + carry;
  vd = res;
})
