require_extension('H');
require_novirt();
require_privilege(get_field(STATE.mstatus->read(), MSTATUS_TVM) ? PRV_M : PRV_S);
MMU.flush_tlb();
