require_extension(EXT_ZVTBASE);
require_vector_novtype(false);

reg_t mtype = insn.rs1() & 0x3;
reg_t vtype = insn.i_imm() & 0x118;

P.VU.msetmtype(mtype, vtype);

VECTOR_END;
