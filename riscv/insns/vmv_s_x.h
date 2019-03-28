// vmv_s_x: vd[0] = rs1

reg_t rd_num = insn.rd(); 
STATE.VU.elt<int32_t>(rd_num, 0) = RS1;
