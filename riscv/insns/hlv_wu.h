require_extension('H');
require_rv64;
require_novirt();
require_privilege(get_field(STATE.hstatus->read(), HSTATUS_HU) ? PRV_U : PRV_S);
WRITE_RD(MMU.guest_load_uint32(RS1));
