reg_t tmp = npc;
set_pc(JUMP_TARGET);
WRITE_RD(tmp);
