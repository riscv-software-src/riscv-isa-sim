# See LICENSE for license details.

  .section .text.init
  .globl _start
_start:
  # MDT resets to 1. With cetrig=0, this must signal a critical error.
  .word 0

  .section .tohost,"aw",@progbits
  .balign 8
  .globl tohost, fromhost
tohost:
  .dword 0
fromhost:
  .dword 0
