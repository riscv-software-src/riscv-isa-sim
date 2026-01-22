.option norvc

.global _entry
.global fromhost
.global tohost

.text
_entry:
  la t0, exception_handler
  csrw mtvec, t0
  csrr t1, mstatus
  // Setting bit number 9 (mstatus.VS)
  li t2, 1
  slli t2, t2, 9
  or t1, t1, t2
  // Setting bit number 13 (mstatus.FS)
  li t3, 1
  slli t3, t3, 13
  or t1, t1, t3
  csrw mstatus, t1

//----------------------------MASK AGNOSTIC TEST-------------------------------
  // Tail undisturbed, mask agnostic with VL = 3
  vsetivli a0, 3, e32, m1, tu, ma
  vmclr.m v0
  // Mask v0 - 0b...001
  vadd.vi v0, v0, 1
  // Fourth element should be zero because of tu
  // If mask fill 1s extension (xspikema1s) enable:
  //   Second and third elements should be 0xffffffff because of 001 mask
  //   v2  0x00000000ffffffffffffffff00000002
  // If mask fill 1s extension (xspikema1s) disable:
  //   Second and third elements should be zero because of 001 mask
  //   v2  0x00000000000000000000000000000002
  vadd.vv v2, v0, v0, v0.t

//----------------------------TAIL AGNOSTIC TEST-------------------------------
  vsetivli a0, 3, e32, m1, ta, mu
  // Second and third elements should be zero because of 001 mask with mu
  // If tail fill 1s extension (xspiketa1s) enable:
  //   Fourth element should be 0xffffffff because of ta
  //   v3  0xffffffff000000000000000000000003
  // If tail fill 1s extension (xspiketa1s) disable:
  //   Fourth element should be zero because of ta
  //   v3  0x00000000000000000000000000000003
  vadd.vv v3, v2, v0, v0.t

//---------------------------MASK AGNOSTIC M4 TEST-----------------------------
  vsetivli a0, 5, e64, m4, tu, ma
  vmclr.m v0
  // Mask v0 - 0b...11001
  vadd.vi v0, v0, -7
  vadd.vv v4, v4, v4, v0.t

//---------------------------TAIL AGNOSTIC M4 TEST-----------------------------
  vsetivli a0, 5, e64, m4, ta, mu
  // Mask v0 - 0b...11001
  vadd.vv v8, v8, v8, v0.t

//---------------------------------END TESTING---------------------------------
  la t0, exit
  jalr t0

exception_handler:
  csrr x10, mcause
  // In case of breakpoint (Interrupt = 0, Exception code = 3) we finalize.
  // Otherwise it's not the expected behavior and we go into an infinite loop.
  li x11, 3
  beq x10, x11, exit
  j infinite_loop

exit:
  li ra, 1
  la sp, tohost
  sd ra, 0(sp)

infinite_loop:
  j infinite_loop

.balign 64
tohost:
.8byte 0x0
.balign 64
fromhost:
.8byte 0x0
