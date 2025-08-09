# Minimal RISC-V vector test for vfmin.vv instruction
# Testing ZVFBFA extension
# Only testing: vfmin.vv (vector-vector floating-point minimum)

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

    # Initialize vector configuration
    li t0, 4                        # Set vl = 4 (small for testing)
    vsetvli t1, t0, e16, m1, ta, ma # Set SEW=16, LMUL=1

    # Test data setup
    la t0, test_data_a
    la t1, test_data_b

    #=================================================================
    # Test: Try basic vector operations first
    #=================================================================

    # Load test data (16-bit floating-point)
    vle16.v v1, (t0)                # Load first operand
    vle16.v v2, (t1)                # Load second operand

    # Set up mask register (all elements enabled)
    li t2, -1                       # All bits set
    vmv.v.x v0, t2                  # Set mask to all 1s

    # Try a simple vector operation first to see if vector works
    vmv.v.v v2, v1

    #=================================================================
    # Test: vfmin.vv (basic test)
    #=================================================================

    # Load second operand
    la t0, test_data_b
    vle16.v v2, (t0)

    # Perform vector floating-point minimum: v3 = min(v1, v2)
    vfmin.vv v3, v1, v2

    # Extract first element from vector register v3 directly
    vmv.x.s t1, v3                  # Extract v3[0] to scalar register t1
    slli t1, t1, 16                 # Shift left 16 bits
    srli t1, t1, 16                 # Shift right 16 bits to mask to 16-bit
    
    # Test 1: Check FP16 result (altfmt=0)
    # Store actual FP16 result for inspection
    mv s2, t1                       # s2 = actual FP16 result

    # Expected: min(1.0, 0.5) = 0.5 in FP16 = 0x3800
    li t2, 0x3800                   # Expected FP16 result
    bne t1, t2, fp16_test_failed    # If doesn't match, FP16 test failed

    # FP16 test passed
    li s1, 0                        # s1 = 0 (FP16 test passed)
    j fp16_test_done

fp16_test_failed:
    li s1, 1                        # s1 = 1 (FP16 test failed)

fp16_test_done:

    #=================================================================
    # Test 2: vfmin.vv with BF16 mode (altfmt=1)
    #=================================================================

    # Initialize vector configuration for BF16 mode (altfmt=1)
    vsetivli t1, 4, 0x1C8           # Set vl=4, SEW=16, LMUL=1, ta, ma, altfmt=1 (BF16)

    # Reload test data for BF16 test
    la t0, test_data_a
    la t1, test_data_b
    vle16.v v1, (t0)                # Load first operand
    vle16.v v2, (t1)                # Load second operand

    # Perform vector floating-point minimum in BF16 mode: v3 = min(v1, v2)
    vfmin.vv v3, v1, v2

    # Extract first element and check format
    vmv.x.s t1, v3                  # Extract v3[0] to scalar register t1
    slli t1, t1, 16                 # Shift left 16 bits
    srli t1, t1, 16                 # Shift right 16 bits to mask to 16-bit

    # Test 2: Check BF16 result (altfmt=1)
    # Store actual BF16 result for inspection with clear markers
    mv s3, t1                       # s3 = actual BF16 result

    # Create a clear signature to identify the actual result in trace
    li s9, 0xDEADBEEF               # s9 = marker before actual result
    mv s10, t1                      # s10 = actual BF16 result (copy)
    li s11, 0xCAFEBABE              # s11 = marker after actual result

    # Test 2: Check BF16 result (altfmt=1)
    # Expected: min(1.0, 0.5) = 0.5 in BF16 = 0x3F00
    li t2, 0x3F00                   # Expected BF16 result
    bne t1, t2, bf16_test_failed    # If doesn't match, BF16 test failed

    # BF16 test passed
    li s7, 0                        # s7 = 0 (BF16 test passed)
    j bf16_test_done

bf16_test_failed:
    li s7, 1                        # s7 = 1 (BF16 test failed)

bf16_test_done:
    # Determine final result based on both tests
    # Both tests must pass for overall PASS
    or t0, s1, s7                   # t0 = s1 OR s7 (0 if both passed, 1 if any failed)
    beqz t0, both_tests_passed      # If both passed, success

    # At least one test failed
    li a0, 1                        # Exit code 1: Test failed
    j exit_test

both_tests_passed:
    li a0, 0                        # Exit code 0: All tests passed
    j exit_test



exit_test:
    # Store final result in register s8 for inspection
    # s8 will contain: 0x50415353 = "PASS" or 0x4641494C = "FAIL"
    beqz a0, store_pass_in_reg      # If exit code 0, store PASS
    j store_fail_in_reg             # Otherwise store FAIL

store_pass_in_reg:
    li s8, 0x50415353               # "PASS" in ASCII (big-endian)
    j final_exit

store_fail_in_reg:
    li s8, 0x4641494C               # "FAIL" in ASCII (big-endian)
    j final_exit

final_exit:
    # Store expected values for easy comparison
    li s4, 0x3800                   # s4 = expected FP16 result (0.5)
    li s5, 0x3F00                   # s5 = expected BF16 result (0.5)

    # Call debug helper to print actual BF16 result
    mv a0, s3                       # a0 = actual BF16 result
    jal ra, print_debug_value       # Print the value

    li a7, 93                       # exit system call number
    ecall                           # Make system call

#=================================================================
# Debug Helper Functions
#=================================================================

# Helper function: Print debug value by making it visible in trace
# Input: a0 = value to print
# This function creates a recognizable pattern in the trace
print_debug_value:
    # Create a signature pattern to identify the value in trace
    li t0, 0xDEADBEEF               # Start marker
    li t1, 0x12345678               # Pattern 1
    mv t2, a0                       # The actual value we want to see
    li t3, 0x87654321               # Pattern 2
    li t4, 0xCAFEBABE               # End marker

    # Make the value visible by using it in arithmetic
    slli t5, a0, 16                 # Shift left
    srli t6, a0, 16                 # Shift right
    add t5, t5, t6                  # Combine (this will show the value)

    ret

.data
.align 4

# Test data for 16-bit floating-point (only 4 elements)
test_data_a:
    .half 0x3C00, 0x4000, 0x4200, 0x4400    # 1.0, 2.0, 3.0, 4.0 in FP16

test_data_b:
    .half 0x3800, 0x3C00, 0x3E00, 0x4000    # 0.5, 1.0, 1.5, 2.0 in FP16

# Expected results for FP16 minimum (for reference)
# test_data_a: 1.0, 2.0, 3.0, 4.0 (0x3C00, 0x4000, 0x4200, 0x4400)
# test_data_b: 0.5, 1.0, 1.5, 2.0 (0x3800, 0x3C00, 0x3E00, 0x4000)
# Expected:    0.5, 1.0, 1.5, 2.0 (0x3800, 0x3C00, 0x3E00, 0x4000)

# Test result stored in register s8:
# s8 = 0x50415353 ("PASS") or s8 = 0x4641494C ("FAIL")
