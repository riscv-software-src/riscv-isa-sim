.option norvc
.section .text
.globl _start
.type _start,@function
_start:
  la t0, trap
  csrw mtvec, t0
  li t0, 0x600
  csrs mstatus, t0
  # vsetivli t1, 1, e8, m1, ta, ma (raw encoding; file advertises rv64i)
  .globl zvl_trigger_instruction
zvl_trigger_instruction:
  .4byte 0xcc00f357
  li t0, 0x11
  j report
trap:
  li t0, 0x22
report:
  la t1, result
  sd t0, 0(t1)
  la t1, tohost
  li t0, 1
  sd t0, 0(t1)
  fence rw, rw
1: j 1b
.section .result,"aw",@progbits
.align 3
.globl result
result: .dword 0
.section .tohost,"aw",@progbits
.align 6
.globl tohost
tohost: .dword 0
.globl fromhost
fromhost: .dword 0
