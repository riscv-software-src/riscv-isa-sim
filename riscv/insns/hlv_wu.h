require_extension('H');
require_rv64;
require_privilege(get_field(STATE.hstatus, HSTATUS_HU) ? PRV_S : PRV_U);
require_novirt();
WRITE_RD(MMU.guest_load_uint32(RS1));
