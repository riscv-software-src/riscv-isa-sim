.section .text.init
.globl _start
_start:
  li t0, 0x0c000004
  li t1, 7
  sw t1, 0(t0)

  li t0, 0x0c201000
  sw zero, 0(t0)

  li t0, 0x0c002080
  li t1, 2
  sw t1, 0(t0)

  li t0, 0x10000001
  li t1, 2
  sb t1, 0(t0)

  li t0, 0x0c001000
1:
  lw t1, 0(t0)
  andi t1, t1, 2
  beqz t1, 1b

  li t0, 1 << 5
  csrrc t1, mip, t0
  andi t1, t1, 1 << 9
  beqz t1, fail

  li t0, 0x10000001
  sb zero, 0(t0)

  li t0, 0x0c001000
2:
  lw t1, 0(t0)
  andi t1, t1, 2
  bnez t1, 2b

  csrr t1, mip
  andi t1, t1, 1 << 9
  bnez t1, fail

  li t1, 1
  j exit

fail:
  li t1, 3

exit:
  lla t0, tohost
  sd t1, 0(t0)
3:
  j 3b

.section .tohost,"aw",@progbits
.balign 64
.globl tohost
tohost:
  .dword 0
.balign 64
.globl fromhost
fromhost:
  .dword 0
