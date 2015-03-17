require_privilege(PRV_S);
switch (get_field(STATE.mstatus, MSTATUS_PRV))
{
  case PRV_S: set_pc(p->get_state()->sepc); break;
  case PRV_M: set_pc(p->get_state()->mepc); break;
  default: abort();
}
p->pop_privilege_stack();
