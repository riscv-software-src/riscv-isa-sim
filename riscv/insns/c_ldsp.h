#ifdef BODY
require_extension(EXT_ZCA);
require((xlen == 64) || p->extension_enabled(EXT_ZCLSD));
require(insn.cildtype.rvc_rd != 0);

if (xlen == 32) {
  WRITE_RD_PAIR(MMU.load<int64_t>(RVC_SP + RVC_LDSP_IMM));
} else {
  WRITE_RD(MMU.load<int64_t>(RVC_SP + RVC_LDSP_IMM));
}

#endif

#define CILDTYPE_INSN
