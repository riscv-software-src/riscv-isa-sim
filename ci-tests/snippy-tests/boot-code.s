.option norvc
.global _entry
.global fromhost
.global tohost
.text
_entry:
  la t0, exception_handler
  csrw mtvec, t0
  la t0, SNIPPY_ENTRY
  jalr t0
  j exit
exception_handler:
  csrr x10, mcause
# In case of breakpoint (Interrupt = 0, Exception code = 3) we finalize.
# Otherwise it's not the expected behavior and we go into an infinite loop.
  li x11, 3
  beq x10, x11, exit
  j infinite_loop
exit:
  li ra, 1
  la sp, tohost
  sw ra, 0(sp)
infinite_loop:
  j infinite_loop
.balign 64
tohost:
.8byte 0x0
fromhost:
.8byte 0x0

