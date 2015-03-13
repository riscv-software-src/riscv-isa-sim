require_privilege(PRV_S);
p->pop_privilege_stack();
set_pc(p->get_state()->sepc);
