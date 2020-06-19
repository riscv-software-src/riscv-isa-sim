require_extension('H');
require_rv64;
require_privilege(get_field(STATE.hstatus, HSTATUS_HU) ? PRV_S : PRV_U);
require_novirt();
MMU.guest_store_uint64(RS1, RS2);
