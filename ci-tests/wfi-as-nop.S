# See LICENSE for license details.

.section .text.init
.globl rvtest_entry_point
rvtest_entry_point:
  # With no locally enabled interrupts, only --wfi-as-nop lets us continue.
  csrw mie, zero
  wfi

  # Report success to Spike.
  li t0, 1
  la t1, tohost
  sw t0, 0(t1)
1:
  j 1b

.section .tohost, "aw", @progbits
.balign 64
.globl tohost
tohost:
  .dword 0
.globl fromhost
fromhost:
  .dword 0
