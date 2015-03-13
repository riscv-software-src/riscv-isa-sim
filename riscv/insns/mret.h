require_privilege(PRV_M);
p->pop_privilege_stack();
set_pc(p->get_state()->mepc);
