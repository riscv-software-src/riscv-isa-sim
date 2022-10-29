require_extension('C');
require_extension('D');
require_fp;
WRITE_FRD(f64(MMU.load<uint64_t>(RVC_SP + insn.rvc_ldsp_imm())));
