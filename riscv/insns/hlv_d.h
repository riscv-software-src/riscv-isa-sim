require_extension('H');
require_rv64;
require_privilege(get_field(STATE.hstatus, HSTATUS_HU) ? PRV_U : PRV_S);
require_novirt();
WRITE_RD(MMU.guest_load_int64(RS1));
