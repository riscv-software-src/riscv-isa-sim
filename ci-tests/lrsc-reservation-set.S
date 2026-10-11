#define RWORD      0x80200000
#define RDWORD     0x80210000
#define RBLOCK     0x80220000

.section .text.init
.globl _start
_start:
  # sc.w to the same word must succeed.
  li       a0, RWORD
  lr.w     a1, (a0)
  li       a2, 0x55aa55aa
  sc.w     t0, a2, (a0)
  bnez     t0, fail

  # a successful sc.w consumes the reservation, so a second sc.w to the same
  # word must fail.
  li       a0, RWORD
  lr.w     a1, (a0)
  li       a2, 0x12345678
  sc.w     t0, a2, (a0)
  bnez     t0, fail
  sc.w     t1, a2, (a0)
  beqz     t1, fail

  # sc.d to the same doubleword must succeed.
  li       a0, RDWORD
  lr.d     a1, (a0)
  li       a2, 0x11223344
  sc.d     t0, a2, (a0)
  bnez     t0, fail

  # lr.w at the start of an aligned block, sc.w to the word
  # 4 bytes further on (still inside the block).  Out of the box (exp=0) this
  # must fail; with an enlarged reservation set it must succeed.
  li       a0, RBLOCK
  lr.w     a1, (a0)
  addi     a3, a0, 64
  li       a2, 0x00007777
  sc.w     t0, a2, (a3)
#ifdef RSNEIGHBOR_SUCCESS
  bnez     t0, fail
#else
  beqz     t0, fail
#endif

  # Pass.
  li       t1, 1
  j        exit

fail:
  li       t1, 3

exit:
  lla      t0, tohost
  sd       t1, 0(t0)
1:
  j        1b

.section .tohost, "aw", @progbits
.balign 64
.globl tohost
tohost:
  .dword 0
.globl fromhost
fromhost:
  .dword 0
