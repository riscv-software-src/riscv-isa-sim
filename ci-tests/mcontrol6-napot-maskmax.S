# mcontrol6 tdata1: type (bits XLEN-1:XLEN-4) = 6 (mcontrol6), m (bit 6) = 1.
# load/store/execute stay 0, so the trigger never fires.
#define MCONTROL6_M  ((6 << (__riscv_xlen - 4)) | (1 << 6))

# match (bits 10:7) = 1: NAPOT, tdata2 encodes a power-of-two address range.
#define MATCH_NAPOT  (1 << 7)

  .section .text.init
  .globl _start
_start:
  # NAPOT all-ones probe: bit maskmax6-1 is cleared.
  li t0, MCONTROL6_M | MATCH_NAPOT
  csrw tdata1, t0
  li t0, -1
  csrw tdata2, t0
  csrr a0, tdata2

  # A range below the maximum is legal and must read back unchanged: a1 = a0.
  csrw tdata2, a0
  csrr a1, tdata2

  # match = 0 (EQUAL): tdata2 is a plain address, no legalization: a2 = all ones.
  li t0, MCONTROL6_M
  csrw tdata1, t0
  li t0, -1
  csrw tdata2, t0
  csrr a2, tdata2

  # stop is linked at 0x81000000, out of jal range, so jump through a register.
  lla t0, stop
  jr t0

  .section .stop,"ax",@progbits
stop:
  j stop
