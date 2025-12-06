require_extension('Q');
require_fp;
uint128_t v = MMU.load<uint128_t>(RS1 + insn.i_imm());
float128_t f = { uint64_t(v), uint64_t(v >> 64) };
WRITE_FRD(f);
