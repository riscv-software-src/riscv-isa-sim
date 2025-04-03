require_extension('D');
require_fp;
WRITE_FRD(f64(MMU.load<uint64_t>(RS1 + insn.i_imm())));
