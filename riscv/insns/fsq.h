require_extension('Q');
require_fp;
uint128_t v = FRS2.v[0] | (uint128_t(FRS2.v[1]) << 64);
MMU.store<uint128_t>(RS1 + insn.s_imm(), v);
