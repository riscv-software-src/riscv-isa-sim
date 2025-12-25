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
  // Mask v0 - 0x00000000000000010000000100000001
  vadd.vi v0, v0, 1
  // Fourth element should be zero because of tu
  // If mask fill 1s extension (xspikema1s) enable:
  //   Second and third elements should be 0xffffffff because of 0001 mask
  //   v2  0x00000000ffffffffffffffff00000002
  // If mask fill 1s extension (xspikema1s) disable:
  //   Second and third elements should be zero because of 0001 mask
  //   v2  0x00000000000000000000000000000002
  vadd.vi v2, v2, 2, v0.t

//----------------------------TAIL AGNOSTIC TEST-------------------------------
  vsetivli a0, 3, e32, m1, ta, mu
  // Second and third elements should be zero because of 0001 mask with mu
  // If tail fill 1s extension (xspiketa1s) enable:
  //   Fourth element should be 0xffffffff because of ta
  //   v3  0xffffffff000000000000000000000003
  // If tail fill 1s extension (xspiketa1s) disable:
  //   Fourth element should be zero because of ta
  //   v3  0x00000000000000000000000000000003
  vadd.vi v3, v3, 3, v0.t

//-----------------------------VCOMPRESS.VM TEST-------------------------------
  // Tail agnostic, mask undisturbed with VL = 8
  vsetivli a0, 8, e8, m1, ta, mu
  vmclr.m v0
  vadd.vi v0, v0, 13
  // Mask v0 - 0b...00001101 => 3 setting bits (of the first 8) => 3 elements
  // packed, remaining elements - tail: v4  0xffffffffffffffffffffffffff000000
  vcompress.vm v4, v1, v0

//-----------------------------MASK LOGICAL TEST-------------------------------
  // Tail undisturbed, mask agnostic with VL = 14
  vsetivli a0, 14, e8, m1, tu, ma
  // Mask destination => unmasked, tail-agnostic
  // v5  0xffffffffffffffffffffffffffffc000
  vmand.mm v5, v5, v5

//--------------------------SCALAR DESTINATION TEST----------------------------
  vsetivli a0, 8, e8, m1, ta, ma
  // t1 = x6, check that v6 hasn't been changed
  vcpop.m t1, v1, v0.t

//-------------------------------VIOTA.M TEST----------------------------------
  // Tail agnostic, mask agnostic with VL = 5
  vsetivli a0, 5, e16, m1, ta, ma
  vmclr.m v0
  vadd.vi v0, v0, 13
  // Mask v0 - 0b...00001101
  // packed, remaining elements - tail: v7  0xffffffffffffffff00000000ffff0000
  viota.m v7, v1, v0.t

//--------------------------------VMV.S.X TEST---------------------------------
  // Only first element updated, remaining elements - tail:
  // v8  0xffffffffffffffffffffffffffff0000
  vmv.s.x v8, sp

//---------------------------------VLM.V TEST----------------------------------
  // Tail agnostic, mask agnostic with VL = 31
  vsetivli a0, 31, e32, m8, tu, ma
  // For all loads, we will use this address of the executable section (so that
  // the bytes are unambiguous) placed in the sp.
  li sp, 0x00000000800000c4
  // Destination register is always written with a tail-agnostic policy.
  // evl = ceil(vl / 8) = 3, eew = 8 => tail starts from third element
  // v9  0xffffffffffffffffffffffffff410113
  vlm.v v9, 0(sp)

//--------------------------VECTOR UNIT-STRIDE TEST----------------------------
  // Tail agnostic, mask agnostic with VL = 5
  vsetivli a0, 5, e8, m1, ta, ma
  vmclr.m v0
  vadd.vi v0, v0, 13
  // Mask v0 - 0b...00001101
  // Masked vector loads do not update inactive elements.
  // eew = 16, sew = 8 ignored.
  // v10 0xffffffffffff000002b1048700000113
  // v11 0xffffffffffffffffffffffffffffffff
  vle16.v v10, 0(sp), v0.t

//----------------------------VECTOR STRIDED TEST------------------------------
  // Tail agnostic, mask agnostic with VL = 3
  vsetivli a0, 3, e16, m1, ta, ma
  // Mask v0 - 0b...00001101
  li tp, 2
  // Masked vector loads do not update inactive elements.
  // eew = 32, sew = 16 ignored.
  // v12 0xffffffff02b10487000000000c410113
  // v13 0xffffffffffffffffffffffffffffffff
  vlse32.v v12, 0(sp), tp, v0.t

//----------------------------VECTOR INDEXED TEST------------------------------
  // Tail agnostic, mask agnostic with VL = 6
  vsetivli a0, 6, e32, m2, ta, ma
  // Mask v0 - 0b...00011101
  li tp, 29
  vmv.s.x v0, tp
  // Masked vector loads do not update inactive elements.
  // eew = sew = 32
  // v14 0x0c4101130c410113000000000c410113
  // v15 0xffffffffffffffff000000000c410113
  vloxei8.v v14, 0(sp), v1, v0.t

//------------------VECTOR UNIT-STRIDE FAULT-ONLY-FIRST TEST-------------------
  // Tail agnostic, mask agnostic with VL = 31
  vsetivli a0, 31, e8, m2, ta, ma
  // Mask v0 - 0b...001111
  li tp, 15
  // For all fault-only-first loads, we will use this address, because a few
  // bytes after it (0x00000000ffffffff), a trap happens.
  vmv.s.x v0, tp
  li tp, 0x00000000fffffffa
  // Load instructions may overwrite active destination vector register group
  // elements past the element index at which the trap is reported.
  // Here element with index 3 raised an exception => vl reduced to 3.
  // eew = 16, sew = 8 ignored.
  // v16 0xffffffffffffffffffff000000000000
  // v17 0xffffffffffffffffffffffffffffffff
  // v18 0xffffffffffffffffffffffffffffffff
  // v19 0xffffffffffffffffffffffffffffffff
  vle16ff.v v16, (tp), v0.t

//----------------------VECTOR UNIT-STRIDE SEGMENT TEST------------------------
  // Tail agnostic, mask agnostic with VL = 6
  vsetivli a0, 6, e32, m2, ta, ma
  // Mask v0 - 0b...0001101
  li t0, 13
  vmv.s.x v0, t0
  // Masked vector loads do not update inactive elements.
  // nf = 3
  // v20 0xffffffff000000000206205700000113
  // v21 0xffffffff0000000055076e0000000c41
  // v22 0xffffffff000000000001b05700000487
  vlseg3e16.v v20, 0(sp), v0.t

//-------------VECTOR UNIT-STRIDE SEGMENT FAULT-ONLY-FIRST TEST----------------
  // Tail agnostic, mask agnostic with VL = 14
  vsetivli a0, 14, e8, m1, ta, ma
  // nf = 2, vl reduced to 3.
  // v24 0xffffffffffffffffffffffffff000000
  // v25 0xffffffffffffffffffffffffff000000
  vlseg2e8ff.v v24, (tp), v0.t

//------------------------VECTOR STRIDED SEGMENT TEST--------------------------
  // Tail agnostic, mask agnostic with VL = 7
  vsetivli a0, 7, e32, m2, ta, ma
  // Mask v0 - 0b...0001101
  // Masked vector loads do not update inactive elements.
  li t1, 8
  // nf = 4
  // v26 0xffff000000000000f557b05700000113
  // v27 0xffff000000000000cc81020600000c41
  // v28 0xffff0000000000000213550700000487
  // v29 0xffff00000000000000200001000002b1
  vlsseg4e16.v v26, 0(sp), t1, v0.t

//------------------------VECTOR INDEXED SEGMENT TEST--------------------------
  // Tail agnostic, mask agnostic with VL = 2
  vsetivli a0, 2, e32, m1, ta, ma
  // Mask v0 - 0b...00001
  vmclr.m v0
  vadd.vi v0, v0, 1
  // Masked vector loads do not update inactive elements.
  // v30 0xffffffffffffffff000000000c410113
  // v31 0xffffffffffffffff0000000002b10487
  vluxseg2ei32.v v30, 0(sp), v1, v0.t

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
