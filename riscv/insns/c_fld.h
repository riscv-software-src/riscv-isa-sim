require_extension('C');
require_extension('D');
require_fp;
WRITE_RVC_FRS2S(MMU.load_int64(RVC_RS1S + insn.rvc_ld_imm()));
