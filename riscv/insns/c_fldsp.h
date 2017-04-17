require_extension('C');
require_extension('D');
require_fp;
WRITE_FRD(f64(MMU.load_uint64(RVC_SP + insn.rvc_ldsp_imm())));
