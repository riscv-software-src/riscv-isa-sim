require_extension('H');
require_privilege(get_field(STATE.hstatus, HSTATUS_HU) ? PRV_U : PRV_S);
require_novirt();
MMU.guest_store_uint32(RS1, RS2);
