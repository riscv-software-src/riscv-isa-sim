require_privilege(get_field(STATE.mstatus, MSTATUS_TVM) ? PRV_M : PRV_S);
MMU.flush_tlb();
