# Test for vfwcvt.f.x.v with SEW=8
# Convert 8-bit integers to 16-bit floats

.section .text
.globl _start

_start:
    # Enable vector and floating-point extensions
    csrr t0, mstatus
    li t1, 0x600
    li t2, 0x6000
    or t0, t0, t1
    or t0, t0, t2
    csrw mstatus, t0

    # Set vector configuration: SEW=8, LMUL=mf2, altfmt=0 for widening
    li t0, 4
    vsetvli t1, t0, e8, mf2, ta, ma

    # Load 8-bit integer test data
    la t0, test_data_8bit
    vle8.v v0, (t0)

    # Test vfwcvt.f.x.v v4, v0 (widens to SEW=16, LMUL=1)
    vfwcvt.f.x.v v4, v0

    # Extract first element
    vmv.x.s t1, v4
    li t3, 0xFFFF
    and t1, t1, t3

    # Store result
    mv s1, t1

    # Check result (42 as int8 -> FP16)
    li t2, 0x5140  # Expected FP16 for 42.0
    bne t1, t2, test_failed

    # Test passed
    li s8, 0x50415353  # "PASS"
    j exit_test

test_failed:
    li s8, 0x4641494C  # "FAIL"

exit_test:
    li a7, 93
    ecall

.section .data
.align 4

test_data_8bit:
    .byte 42, 24, 18, 81
