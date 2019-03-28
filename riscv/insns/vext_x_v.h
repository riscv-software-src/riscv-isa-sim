// vext_x_v: rd = vs2[rs1]
reg_t vs2_num = insn.rs2(); 
WRITE_RD(STATE.VU.elt<int32_t>(vs2_num, RS1));
