require_extension('C');
reg_t tmp = npc;
set_pc(RVC_RS1 & ~reg_t(1));
WRITE_RD(tmp);
