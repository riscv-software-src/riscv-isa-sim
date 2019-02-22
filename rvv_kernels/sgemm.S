# RV64IDV system
#
# void
# sgemm_nn(size_t n,
#          size_t m,
#          size_t k,
#          const float*a,   // m * k matrix
#          size_t lda,
#          const float*b,   // k * n matrix
#          size_t ldb,
#          float*c,         // m * n matrix
#          size_t ldc)
#
#  c += a*b (alpha=1, no transpose on input matrices)
#  matrices stored in C row-major order

#define FRAMESIZE 32
#define S0_OFFSET 0    
#define S1_OFFSET 8    
#define S2_OFFSET 16    
#define S3_OFFSET 24    
#define CSTRIDE_OFFSET 32    

#define n a0
#define m a1
#define k a2
#define ap a3
#define astride a4
#define bp a5
#define bstride a6
#define cp a7
#define cstride t0
#define kt t1
#define nt t2
#define bnp t3
#define cnp t4
#define akp t5
#define bkp s0
#define nvl s1
#define ccp s2
#define amp s3
    
# Use args as additional temporaries
#define ft12 fa0
#define ft13 fa1
#define ft14 fa2
#define ft15 fa3

# This version holds a 16*VLMAX block of C matrix in vector registers
# in inner loop, but otherwise does not cache or TLB tiling.

sgemm_nn:
    addi sp, sp, -FRAMESIZE
    sd s0, S0_OFFSET(sp)
    sd s1, S1_OFFSET(sp)
    sd s2, S2_OFFSET(sp)
    sd s3, S3_OFFSET(sp)

    # Check for zero size matrices        
    beqz n, exit
    beqz m, exit
    beqz k, exit

    # Convert elements strides to byte strides.
    ld cstride, CSTRIDE_OFFSET(sp)   # Get arg from stack frame
    slli astride, astride, 2
    slli bstride, bstride, 2
    slli cstride, cstride, 2

    slti t6, m, 16
    bnez t6, end_rows

c_row_loop: # Loop across rows of C blocks

    mv nt, n  # Initialize n counter for next row of C blocks

    mv bnp, bp # Initialize B n-loop pointer to start
    mv cnp, cp # Initialize C n-loop pointer

c_col_loop: # Loop across one row of C blocks
    vsetvli nvl, nt, e32  # 32-bit vectors, LMUL=1

    mv akp, ap   # reset pointer into A to beginning
    mv bkp, bnp # step to next column in B matrix

    # Initalize current C submatrix block from memory.
    vlw.v  v0, (cnp); add ccp, cnp, cstride;
    vlw.v  v1, (ccp); add ccp, ccp, cstride;
    vlw.v  v2, (ccp); add ccp, ccp, cstride;
    vlw.v  v3, (ccp); add ccp, ccp, cstride;
    vlw.v  v4, (ccp); add ccp, ccp, cstride;
    vlw.v  v5, (ccp); add ccp, ccp, cstride;
    vlw.v  v6, (ccp); add ccp, ccp, cstride;
    vlw.v  v7, (ccp); add ccp, ccp, cstride;
    vlw.v  v8, (ccp); add ccp, ccp, cstride;
    vlw.v  v9, (ccp); add ccp, ccp, cstride;
    vlw.v v10, (ccp); add ccp, ccp, cstride;
    vlw.v v11, (ccp); add ccp, ccp, cstride;
    vlw.v v12, (ccp); add ccp, ccp, cstride;
    vlw.v v13, (ccp); add ccp, ccp, cstride;
    vlw.v v14, (ccp); add ccp, ccp, cstride;
    vlw.v v15, (ccp); add ccp, ccp, cstride;


    mv kt, k # Initialize inner loop counter

    # Inner loop scheduled assuming 4-clock occupancy of vfmacc instruction and single-issue pipeline
    # Software pipeline loads
    flw ft0, (akp); add amp, akp, astride;
    flw ft1, (amp); add amp, amp, astride;
    flw ft2, (amp); add amp, amp, astride;
    flw ft3, (amp); add amp, amp, astride;
    # Get vector from B matrix
    vlw.v v16, (bkp)

    # Loop on inner dimension for current C block
 k_loop:
    vfmacc.vf v0, v16, ft0
    add bkp, bkp, bstride
    flw ft4, (amp)
    add amp, amp, astride
    vfmacc.vf v1, v16, ft1
    addi kt, kt, -1    # Decrement k counter
    flw ft5, (amp)
    add amp, amp, astride
    vfmacc.vf v2, v16, ft2
    flw ft6, (amp)
    add amp, amp, astride
    flw ft7, (amp)
    vfmacc.vf v3, v16, ft3
    add amp, amp, astride
    flw ft8, (amp)
    add amp, amp, astride
    vfmacc.vf v4, v16, ft4
    flw ft9, (amp)
    add amp, amp, astride
    vfmacc.vf v5, v16, ft5
    flw ft10, (amp)
    add amp, amp, astride
    vfmacc.vf v6, v16, ft6
    flw ft11, (amp)
    add amp, amp, astride
    vfmacc.vf v7, v16, ft7
    flw ft12, (amp)
    add amp, amp, astride
    vfmacc.vf v8, v16, ft8
    flw ft13, (amp)
    add amp, amp, astride
    vfmacc.vf v9, v16, ft9
    flw ft14, (amp)
    add amp, amp, astride
    vfmacc.vf v10, v16, ft10
    flw ft15, (amp)
    add amp, amp, astride
    addi akp, akp, 4            # Move to next column of a
    vfmacc.vf v11, v16, ft11
    beqz kt, 1f                 # Don't load past end of matrix
    flw ft0, (akp)
    add amp, akp, astride
1:  vfmacc.vf v12, v16, ft12
    beqz kt, 1f
    flw ft1, (amp)
    add amp, amp, astride
1:  vfmacc.vf v13, v16, ft13
    beqz kt, 1f
    flw ft2, (amp)
    add amp, amp, astride
1:  vfmacc.vf v14, v16, ft14
    beqz kt, 1f                 # Exit out of loop
    flw ft3, (amp)
    add amp, amp, astride
    vfmacc.vf v15, v16, ft15
    vlw.v v16, (bkp)            # Get next vector from B matrix, overlap loads with jump stalls
    j k_loop

1:  vfmacc.vf v15, v16, ft15
    
    # Save C matrix block back to memory
    vsw.v  v0, (cnp); add ccp, cnp, cstride;
    vsw.v  v1, (ccp); add ccp, ccp, cstride;
    vsw.v  v2, (ccp); add ccp, ccp, cstride;
    vsw.v  v3, (ccp); add ccp, ccp, cstride;
    vsw.v  v4, (ccp); add ccp, ccp, cstride;
    vsw.v  v5, (ccp); add ccp, ccp, cstride;
    vsw.v  v6, (ccp); add ccp, ccp, cstride;
    vsw.v  v7, (ccp); add ccp, ccp, cstride;
    vsw.v  v8, (ccp); add ccp, ccp, cstride;
    vsw.v  v9, (ccp); add ccp, ccp, cstride;
    vsw.v v10, (ccp); add ccp, ccp, cstride;
    vsw.v v11, (ccp); add ccp, ccp, cstride;
    vsw.v v12, (ccp); add ccp, ccp, cstride;
    vsw.v v13, (ccp); add ccp, ccp, cstride;
    vsw.v v14, (ccp); add ccp, ccp, cstride;
    vsw.v v15, (ccp); add ccp, ccp, cstride;

    # Following tail instructions should be scheduled earlier in free slots during C block save.
    # Leaving here for clarity.

    # Bump pointers for loop across blocks in one row
    add cnp, cnp, nvl                        # Move C block pointer over
    add bnp, bnp, nvl                        # Move B block pointer over
    sub nt, nt, nvl                          # Decrement element count in n dimension
    bnez nt, c_col_loop                      # Any more to do?

    # Move to next set of rows
    addi m, m, -16  # Did 16 rows above
    slli t6, astride, 4  # Multiply astride by 16
    add ap, ap, t6         # Move A matrix pointer down 16 rows
    slli t6, cstride, 4  # Multiply cstride by 16
    add cp, cp, t6         # Move C matrix pointer down 16 rows
    
    slti t6, m, 16
    beqz t6, c_row_loop

    # Handle end of matrix with fewer than 16 rows.
    # Can use smaller versions of above decreasing in powers-of-2 depending on code-size concerns.
end_rows:
    # Not done.

exit:
    ld s0, S0_OFFSET(sp)
    ld s1, S1_OFFSET(sp)
    ld s2, S2_OFFSET(sp)
    ld s3, S3_OFFSET(sp)
    addi sp, sp, FRAMESIZE
    ret
