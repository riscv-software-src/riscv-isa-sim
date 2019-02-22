#
# kr636 kernel
# performs stream of <6,3> x <3,6> matrix multiplies
#
# long stream of C[6,6] = A[6,3] * B[3,6] matrix multiplies
#
#                B00 B01 B02 B03 B04 B05
#                B10 B11 B12 B13 B14 B15
#                B20 B21 B22 B23 B24 B25
#
#   A00 A01 A02  C00 C01 C02 C03 C04 C05
#   A10 A11 A12  C10 C11 C12 C13 C14 C15
#   A20 A21 A22  C20 C21 C22 C23 C24 C25
#   A30 A31 A32  C30 C31 C32 C33 C34 C35
#   A40 A41 A42  C40 C41 C42 C43 C44 C45
#   A50 A51 A52  C50 C51 C52 C53 C54 C55
#
# Loads  2 * 3 * 6 * 4B = 36 * 4B
# Stores  6 * 6 * 4B = 36 * 4B

# FMA 6 * 6 * 3 108
# FLOPS 6 * 6 * (3 MUL +  2 ADD) = 180

# FLOPS/B = 180 / ((36+36)*4) = 0.625

# C prototype
# void kr636(size_t n, const float* ap, const float* bp, float* cp)

#define n a0
#define ap a1
#define bp a2
#define cp a3
#define nstrip a4
#define astride a5
#define bstride a6
#define cstride a7
#define arp t7
#define brp t6
#define crp t5
#define niter t4

#define ac0  v18
#define ac1  v19
#define ac2  v20
#define ac3  v21
#define ac4  v22
#define ac5  v23
#define ac6  v24
#define ac7  v25
#define ac8  v26
#define ac9  v27
#define ac10 v28
#define ac11 v29
#define ac12 v30
#define ac13 v31

#define b00 v0
#define b01 v1
#define b02 v2
#define b03 v3
#define b04 v4
#define b05 v5
#define b10 v6
#define b11 v7
#define b12 v8
#define b13 v9
#define b14 v10
#define b15 v11
#define b20 v12
#define b21 v13
#define b22 v14
#define b23 v15
#define b24 v16
#define b25 v17

/*

    Fully unroll the three iteration loop over rows, some software
    pipeline and internal scheduling.

    Use rolling window of contiguous registers for A and C values
    taken from same register pool to remove WAR hazards given limited
    number of arch registers and in-order implementation.

    Schedule of using registers for A and C rows, designed to software pipeline

          ac   00 01 02 03 04 05 06 07 08 09 10 11 12 13
                                 a0 a1 a2                   # Load segment
                              a2 a0 a1                      # Move a2
       row0/3                 a2 a0 a1 c0 c1 c2 c3 c4 c5
       row1/4     a0 a1 a2 c0 c1 c2 c3 c4 c5
       row2/5  c0 c1 c2 c3 c4 c5                a0 a1 a2
                                 a0 a1 a2                   # Load segment

    Repeated twice to do all 6 rows in each matrix stripmine to save code size

*/

    .text
    .global kr636

kr636:
    li astride, 6*3*4   # Byte stride between A matrices
    li bstride, 3*6*4   # Byte stride between B matrices
    li cstride, 6*6*4   # Byte stride between C matrices

.Lmatrix_loop:
    vsetvli nstrip, n, e32

    vlsseg3w.v ac6, (ap), astride # Bring in A[0][*] to ac6-8
    addi arp, ap, 3*4

    mv crp, cp
    li niter, 1

    vlsseg6w.v b00, (bp), bstride     # Bring in B[0][*]
    addi brp, bp, 6*4

.Lrow_loop:
    beqz niter, .LskipB1            # Bring in B[1][*] on first iteration
    vlsseg6w.v b10, (brp), bstride
.LskipB1:
    addi brp, brp, 6*4

    # Accumulate C[0/3][*] in ac8-ac13
    # pushed ac8->ac5 use down to avoid WAR conflict with store of C[2] segment
    vfmul.vv  ac9, ac6, b01
    vfmul.vv ac10, ac6, b02
    vfmul.vv ac11, ac6, b03
    vfmul.vv ac12, ac6, b04
    vfmul.vv ac13, ac6, b05

    vfmacc.vv  ac9, ac7, b11
    vfmacc.vv ac10, ac7, b12

    beqz niter, .LskipB2            # Bring in B[2][*] on first iteration
    vlsseg6w.v b20, (brp), bstride
.LskipB2:

    vmv.v     ac5, ac8        # Move out of way of C segment accumulate
    vfmul.vv  ac8, ac6, b00
    vfmacc.vv ac11, ac7, b13
    vfmacc.vv  ac8, ac7, b10
    vfmacc.vv ac12, ac7, b14
    vfmacc.vv ac13, ac7, b15

    vlsseg3w.v ac1, (arp), astride  # Bring in A[1/4][*] to ac1-ac3
    addi arp, arp, 3*4

    vfmacc.vv  ac8, ac5, b20
    vfmacc.vv  ac9, ac5, b21
    vfmacc.vv ac10, ac5, b22
    vfmacc.vv ac11, ac5, b23
    vfmacc.vv ac12, ac5, b24
    vfmacc.vv ac13, ac5, b25

    # Accumulate C[1/4][*] in ac4-ac9
    vfmul.vv ac4, ac1, b00
    vfmul.vv ac5, ac1, b01

    vssseg6w.v ac8, (crp), cstride     # Store C[0/3][*] from ac8-ac13
    addi crp, crp, 6*4

    vfmul.vv ac6, ac1, b02
    vfmul.vv ac7, ac1, b03

    vfmacc.vv ac4, ac2, b10
    vfmacc.vv ac5, ac2, b11
    vfmacc.vv ac6, ac2, b12
    vfmacc.vv ac7, ac2, b13

    vfmacc.vv ac4, ac3, b20
    vfmacc.vv ac5, ac3, b21
    vfmacc.vv ac6, ac3, b22
    vfmacc.vv ac7, ac3, b23

    vlsseg3w.v ac11, (arp), astride   # Bring in A[2/5][*] to ac11-ac13
    addi arp, arp, 3*4

    # Pushed ac8-ac9 down to avoid WAR conflicts with C[0/3] segement store.
    vfmul.vv ac8, ac1, b04
    vfmul.vv ac9, ac1, b05
    vfmacc.vv ac8, ac2, b14
    vfmacc.vv ac9, ac2, b15
    vfmacc.vv ac8, ac3, b24
    vfmacc.vv ac9, ac3, b25

    # Accumulate C[2/5][*] in ac0-ac5
    vfmul.vv ac0, ac11, b00
    vfmul.vv ac1, ac11, b01

    vssseg6w.v ac4, (crp), cstride  # Store C[1/4][*] from ac4-ac9
    addi crp, crp, 6*4

    vfmul.vv ac2, ac11, b02
    vfmul.vv ac3, ac11, b03

    vfmacc.vv ac0, ac12, b10
    vfmacc.vv ac1, ac12, b11
    vfmacc.vv ac2, ac12, b12
    vfmacc.vv ac3, ac12, b13

    vfmacc.vv ac0, ac13, b20
    vfmacc.vv ac1, ac13, b21
    vfmacc.vv ac2, ac13, b22
    vfmacc.vv ac3, ac13, b23

    beqz niter, .LA3skip
    vlsseg3w.v ac6, (arp), astride  # Bring in A[3][*] to ac6-8
.LA3skip:
    addi arp, arp, 3*4

    # Pushed ac4-ac5 accumulates down to avoid WAR conflicts with C[1/4] segment store
    vfmul.vv ac4, ac11, b04
    vfmul.vv ac5, ac11, b05
    vfmacc.vv ac4, ac12, b14
    vfmacc.vv ac5, ac12, b15
    vfmacc.vv ac4, ac13, b24
    vfmacc.vv ac5, ac13, b25

    vssseg6w.v ac0, (crp), cstride     # Store C[2/5][*] from ac0-ac5
    addi crp, crp, 6*4

    addi niter, niter, -1
    bgez niter, .Lrow_loop


    mul t0, nstrip, astride
    mul t1, nstrip, bstride
    mul t2, nstrip, cstride
    add ap, ap, t0
    add bp, bp, t1
    add cp, cp, t2

    sub n, n, nstrip
    bnez n, .Lmatrix_loop

exit:
    ret


/*

    Perf analysis for standard E27, 128b mem, 128b datapath, 4*FMA, VLMAX=16x32b

    vlsseg3w.v ac6   24 cycles
    vlsseg6w.v b00   32 cycles
    vlsseg6w.v b10   32 cycles
    vlsseg6w.v b20   32 cycles
    vlsseg3w.v ac1   24 cycles
    vssseg6w.v ac8   32 cycles
       vfmacs         8 cycles
    vlsseg3w.v ac11  24 cycles
       vfmacs         8 cycles
    vssseg6w.v ac4   32 cycles
       vfmacs         8 cycles
    vlsseg3w.v ac6   24 cycles
    vssseg6w.v ac0   32 cycles loop back
       vfmacs        20 cycles
    vlsseg3w.v ac1   24 cycles
    vssseg6w.v ac8   32 cycles
       vfmacs         8 cycles
    vlsseg3w.v ac11  24 cycles
       vfmacs         8 cycles
    vssseg6w.v ac4   32 cycles
       vfmacs        32 cycles
    vssseg6w.v ac0   32 cycles drop through
    
                    524 cycles

    524 cycles to perform 16 matrix multiplies
    
    16*180 FLOPS = 2880 FLOPS in 524 cycles
    
    16 * 108 FMA = 1728 FMA in 524 cycles, utilization is  3.3 FMA/cycle out of 4, 82.5%

    Memory bandwidth is 16*((36+36)*4) = 4608 B / vector, in 524 cycles = 8.8 B/cycle

*/
