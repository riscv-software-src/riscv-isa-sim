# Simple test for vfncvt.f.f.q instruction
# Just test one instruction with minimal setup

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
    # This way: source EMUL=4×LMUL=4, destination EMUL=LMUL=1
    li t0, 4
    vsetvli t1, t0, e32, m1, ta, ma   # SEW=8, LMUL=1

    # Load FP32 data into v4 with EMUL=4 (v4 is aligned to 4)
    # Temporarily set SEW=32, LMUL=4 to load FP32 values
    vsetvli t1, t0, e32, m4, ta, ma  # SEW=32, LMUL=4 for loading
    la t0, test_data_fp32
    vle32.v v4, (t0)                # Load FP32 into v4 (EMUL=4)

    # Set back to SEW=8, LMUL=1 for the conversion
    vsetvli t1, t0, e32, m1, ta, ma   # SEW=8, LMUL=1 for conversion

    # Set up mask register first
    li t2, -1
    vmv.v.x v0, t2                  # Set mask to all 1s

    # Test vfncvt.f.f.q v0, v4 (v4=source EMUL=4, v0=dest EMUL=1)
    # Use masked version first: 0x48249057
    .word 0x48249057                # vfncvt.f.f.q v0, v4, v0.t (masked)

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

# This test just verifies that vfncvt.f.f.q doesn't cause illegal instruction
