require_extension('H');
require_novirt();
require_privilege(get_field(STATE.hstatus, HSTATUS_HU) ? PRV_U : PRV_S);
WRITE_RD(MMU.guest_load_x_uint16(RS1));
