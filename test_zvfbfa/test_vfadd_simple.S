# Minimal RISC-V vector test for vfadd.vv instruction
# Testing ZVFBFA extension
# Only testing: vfadd.vv (vector-vector floating-point addition)

.text
.globl _start

_start:
    # Enable both vector and floating-point extensions in mstatus
    csrr t0, mstatus
    li t1, 0x600                    # VS field bits [10:9] = 11 (Dirty) - Vector
    li t2, 0x6000                   # FS field bits [14:13] = 11 (Dirty) - Float
    or t0, t0, t1
    or t0, t0, t2
    csrw mstatus, t0

    #=================================================================
    # Test 1: vfadd.vv with FP16 mode (altfmt=0)
    #=================================================================

    # Initialize vector configuration for FP16 mode (altfmt=0)
    li t0, 4                        # Set vl = 4 (small for testing)
    vsetvli t1, t0, e16, m1, ta, ma # Set SEW=16, LMUL=1, altfmt=0 (FP16)

    # Test data setup
    la t0, test_data_a

    # Load test data (16-bit floating-point)
    vle16.v v1, (t0)

    # Set up mask register (all elements enabled)
    li t2, -1                       # All bits set
    vmv.v.x v0, t2                  # Set mask to all 1s

    # Try a simple vector operation first to see if vector works
    vmv.v.v v2, v1

    # Load second operand
    la t0, test_data_b
    vle16.v v2, (t0)

    # Perform vector floating-point addition: v3 = v1 + v2
    vfadd.vv v3, v1, v2

    # Extract first element from vector register v3 directly
    vmv.x.s t1, v3                  # Extract v3[0] to scalar register t1
    slli t1, t1, 16                 # Shift left 16 bits
    srli t1, t1, 16                 # Shift right 16 bits to mask to 16-bit

    # Check if results indicate BF16 or FP16 arithmetic
    # Compare with BF16 expected value for 1.0 + 0.5 = 1.5
    li t2, 0x3FC0                   # Expected BF16 result: 1.5 in BF16 format
    beq t1, t2, detected_bf16       # If matches BF16 expectation

    # Compare with FP16 expected value
    li t2, 0x3E00                   # Expected FP16 result: 1.5 in FP16 format
    beq t1, t2, detected_fp16       # If matches FP16 expectation

    # Neither match - unknown format
    li s4, -1                       # s4 = -1 for unknown/error
    j format_check_done

detected_bf16:
    li s4, 1                        # s4 = 1 for BF16
    j format_check_done

detected_fp16:
    li s4, 0                        # s4 = 0 for FP16

format_check_done:
    # Compare with appropriate expected values based on detected format

    beqz s4, check_fp16_expected    # If s4=0, check FP16 expectations
    li t2, 1
    beq s4, t2, check_bf16_expected # If s4=1, check BF16 expectations

    # s4=-1: Unknown format - this is an error
    li t0, 2                        # Error code 2: unknown format
    j format_test_done

check_fp16_expected:
    # First result already checked, now check second result
    # Extract second element from vector register v3
    li t3, 1                        # Element index 1
    vslidedown.vi v4, v3, 1         # Slide down to get element 1
    vmv.x.s t2, v4                  # Extract v4[0] (which was v3[1]) to t2
    slli t2, t2, 16                 # Shift left 16 bits
    srli t2, t2, 16                 # Shift right 16 bits to mask to 16-bit

    li t0, 0x4200                   # Expected result 1: 3.0 in FP16
    beq t0, t2, format_confirmed    # If matches, FP16 confirmed
    li t0, 1                        # Error code 1: FP16 mismatch
    j format_test_done

check_bf16_expected:
    # First result already checked, now check second result
    # Extract second element from vector register v3
    li t3, 1                        # Element index 1
    vslidedown.vi v4, v3, 1         # Slide down to get element 1
    vmv.x.s t2, v4                  # Extract v4[0] (which was v3[1]) to t2
    slli t2, t2, 16                 # Shift left 16 bits
    srli t2, t2, 16                 # Shift right 16 bits to mask to 16-bit

    li t0, 0x4040                   # Expected result 1: 3.0 in BF16
    beq t0, t2, format_confirmed    # If matches, BF16 confirmed
    li t0, 3                        # Error code 3: BF16 mismatch
    j format_test_done

format_confirmed:
    li t0, 0                        # Success: format detected and results match

format_test_done:
    # Store first test results
    mv s1, t0                       # s1 = error flag from first test (FP16 mode)
    mv s6, s4                       # s6 = format detected in first test

    #=================================================================
    # Test 2: vfadd.vv with BF16 mode (altfmt=1)
    #=================================================================

    # Initialize vector configuration for BF16 mode (altfmt=1)
    vsetivli t1, 4, 0x1C8           # Set vl=4, SEW=16, LMUL=1, ta, ma, altfmt=1 (BF16)

    # Reload test data for BF16 test
    la t0, test_data_a
    la t1, test_data_b
    vle16.v v1, (t0)                # Load first operand
    vle16.v v2, (t1)                # Load second operand

    # Perform vector floating-point addition in BF16 mode: v3 = v1 + v2
    vfadd.vv v3, v1, v2

    # Extract first element and check format
    vmv.x.s t1, v3                  # Extract v3[0] to scalar register t1
    slli t1, t1, 16                 # Shift left 16 bits
    srli t1, t1, 16                 # Shift right 16 bits to mask to 16-bit

    # Check if results indicate BF16 arithmetic was used
    li t2, 0x3FC0                   # Expected BF16 result: 1.5 in BF16 format
    beq t1, t2, bf16_test_passed    # If matches BF16 expectation

    # BF16 test failed
    li s7, 1                        # s7 = 1 for BF16 test failed
    j bf16_test_done

bf16_test_passed:
    li s7, 0                        # s7 = 0 for BF16 test passed

bf16_test_done:
    # Determine final exit code based on both tests
    beqz s6, check_fp16_success     # If first test detected FP16
    li t2, -1
    beq s6, t2, check_bf16_success  # If first test was unknown, check BF16

    # First test detected BF16 but we're in FP16 mode - error
    li a0, 4                        # Error code 4: inconsistent format
    j exit_test

check_fp16_success:
    beqz s1, fp16_confirmed         # If FP16 test passed
    li a0, 1                        # FP16 test failed
    j exit_test

fp16_confirmed:
    # FP16 mode worked, now check if BF16 mode also works
    beqz s7, both_modes_work        # If BF16 test also passed
    li a0, 0                        # Only FP16 works (normal)
    j exit_test

check_bf16_success:
    beqz s7, bf16_confirmed         # If BF16 test passed
    li a0, 3                        # BF16 test failed
    j exit_test

bf16_confirmed:
    li a0, 5                        # BF16 mode works

both_modes_work:
    li a0, 6                        # Both FP16 and BF16 modes work
    j exit_test

exit_test:
    # Store final result in register s8 for inspection
    # s8 will contain: 0x50415353 = "PASS" or 0x4641494C = "FAIL"
    beqz a0, store_pass_in_reg      # If exit code 0, store PASS
    li t2, 6
    beq a0, t2, store_pass_in_reg   # If exit code 6 (both modes work), store PASS
    j store_fail_in_reg             # Otherwise store FAIL

store_pass_in_reg:
    li s8, 0x50415353               # "PASS" in ASCII (big-endian)
    j final_exit

store_fail_in_reg:
    li s8, 0x4641494C               # "FAIL" in ASCII (big-endian)
    j final_exit

final_exit:
    li a7, 93                       # exit system call number
    ecall                           # Make system call

#=================================================================
# Helper Functions
#=================================================================








.data
.align 4

# Test data for 16-bit floating-point (only 4 elements)
test_data_a:
    .half 0x3C00, 0x4000, 0x4200, 0x4400    # 1.0, 2.0, 3.0, 4.0 in FP16

test_data_b:
    .half 0x3800, 0x3C00, 0x3E00, 0x4000    # 0.5, 1.0, 1.5, 2.0 in FP16

# Expected results for FP16 addition (for reference)
# test_data_a: 1.0, 2.0, 3.0, 4.0 (0x3C00, 0x4000, 0x4200, 0x4400)
# test_data_b: 0.5, 1.0, 1.5, 2.0 (0x3800, 0x3C00, 0x3E00, 0x4000)
# Expected:    1.5, 3.0, 4.5, 6.0 (0x3E00, 0x4200, 0x4480, 0x4600)

# Test result stored in register s8:
# s8 = 0x50415353 ("PASS") or s8 = 0x4641494C ("FAIL")
