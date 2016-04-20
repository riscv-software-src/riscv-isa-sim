switch (STATE.prv)
{
  case PRV_U: throw trap_user_ecall();
  case PRV_S: throw trap_supervisor_ecall();
  case PRV_H: throw trap_hypervisor_ecall();
  case PRV_M: throw trap_machine_ecall();
}
