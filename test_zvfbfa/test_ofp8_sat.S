# Simple test for vfncvt.sat.f.f.q instruction
# Tests the saturation version of FP32 to OFP8 conversion

.section .text
.globl _start

_start:
    # Enable vector and floating-point extensions
    csrr t0, mstatus
    li t1, 0x600                    # Set FS=01 (initial)
    li t2, 0x6000                   # Set VS=01 (initial)  
    or t0, t0, t1
    or t0, t0, t2
    csrw mstatus, t0

    # Set vector configuration: SEW=8, LMUL=1, altfmt=0 (E4M3)
    # Source v4 will have EMUL=4×LMUL=4, dest v0 will have EMUL=LMUL=1
    li t0, 4
    vsetvli t1, t0, e8, m1, ta, ma   # SEW=8, LMUL=1

    # Load FP32 data into v4 (v4 is aligned to 4 registers)
    # Temporarily set SEW=32, LMUL=1 to load exactly 4 FP32 values
    vsetvli t1, t0, e32, m1, ta, ma  # SEW=32, LMUL=1 for loading
    la t0, test_data_fp32
    vle32.v v4, (t0)                # Load FP32 into v4 (LMUL=1)

    # Set back to SEW=8, LMUL=1 for the conversion (not LMUL=4!)
    vsetvli t1, t0, e8, m1, ta, ma   # SEW=8, LMUL=1 for conversion

    # Test vfncvt.sat.f.f.q v0, v4 (v4=source aligned to 4, v0=dest)
    # Change vs2 from v0 to v4: 0x480d9057 -> 0x4824d957
    .word 0x4824d957                # vfncvt.sat.f.f.q v0, v4, v0.t

    # Check if we get here without illegal instruction
    li s1, 0                        # Success flag

    # Store result
    li s8, 0x50415353               # "PASS"
    j exit_test

exit_test:
    li a7, 93                       # exit system call number
    ecall                           # Make system call

.section .data
.align 4

# Simple test data: just 1.0 in FP32
test_data_fp32:
    .word 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000    # 1.0, 1.0, 1.0, 1.0

# This test verifies that vfncvt.sat.f.f.q doesn't cause illegal instruction
