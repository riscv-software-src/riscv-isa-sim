# mcontrol6: m = 1, execute = 1, action = 0 (breakpoint exception).
#define MCONTROL6_EXEC_M ((6 << (__riscv_xlen - 4)) | (1 << 6) | (1 << 2))

  .section .text.init
  .globl _start
_start:
  li t0, MCONTROL6_EXEC_M
  csrw tdata1, t0
  lla t0, target
  csrw tdata2, t0

  # MIE = 0: the trigger must not fire, target gets cached.
  lla t0, fail
  csrw mtvec, t0
  call target

  # MIE = 1: the trigger must fire now.
  lla t0, pass
  csrw mtvec, t0
  csrsi mstatus, 8
  call target

  .balign 4
fail:
  li t1, 3
  j exit
  .balign 4
pass:
  li t1, 1
exit:
  lla t0, tohost
  sw t1, 0(t0)
1:
  j 1b

target:
  ret

.section .tohost,"aw",@progbits
.globl tohost
tohost:
  .dword 0
.globl fromhost
fromhost:
  .dword 0
