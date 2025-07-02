require_extension(EXT_ZCF);
require_fp;
WRITE_FRD(f32(MMU.load<uint32_t>(RVC_SP + insn.rvc_lwsp_imm())));
