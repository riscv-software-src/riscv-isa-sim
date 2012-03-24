require_supervisor;
if(sr & SR_ET)
  throw trap_illegal_instruction;
set_pcr(PCR_SR, ((sr & SR_PS) ? sr : (sr & ~SR_S)) | SR_ET);
set_pc(epc);
