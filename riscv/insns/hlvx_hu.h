require_extension('H');
require_privilege(get_field(STATE.hstatus, HSTATUS_HU) ? PRV_S : PRV_U);
require_novirt();
WRITE_RD(MMU.guest_load_x_uint16(RS1));
