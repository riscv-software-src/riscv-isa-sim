# Dummy RISC-V vector test - just initialize vector unit and exit
# This is the minimal test to verify vector unit setup works

.text
.globl _start

_start:
    # Try to enable vector extension in mstatus first
    # mstatus.VS = 11 (Dirty) to enable vector extension
    csrr t0, mstatus
    li t1, 0x600                    # VS field bits [10:9] = 11 (Dirty)
    or t0, t0, t1
    csrw mstatus, t0

    # Now try vector configuration
    li t0, 4                        # Set vl = 4
    vsetvli t1, t0, e8, m1, ta, ma  # Set SEW=8, LMUL=1, tail agnostic, mask agnostic

    # Exit successfully
    li a0, 0                        # Exit code 0 (success)
    li a7, 93                       # exit system call number
    ecall                           # Make system call

# No data section needed for this dummy test
