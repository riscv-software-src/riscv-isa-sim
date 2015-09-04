require_extension('C');
require_extension('D');
require_fp;
WRITE_FRD(MMU.load_int64(RVC_SP + insn.rvc_ldsp_imm()));
