reg_t temp = RS1;
RD = npc;
set_pc((temp + SIMM) & ~1);
