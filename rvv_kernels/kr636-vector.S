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
#define arp t6
#define brp t5
#define crp t4
#define nrows t3

#define aa0 v24
#define aa1 v25
#define aa2 v26

#define cc0 v18
#define cc1 v19
#define cc2 v20
#define cc3 v21
#define cc4 v22
#define cc5 v23

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


    .text
    .global kr636

kr636:
    li astride, 6*3*4   # Byte stride between A matrices
    li bstride, 3*6*4   # Byte stride between B matrices
    li cstride, 6*6*4   # Byte stride between C matrices
    
.Lmatrix_loop:
    vsetvli nstrip, n, e32

    # Bring in all of B matrix
    vlsseg6w.v b00, (bp), bstride
    addi brp, bp, 6*4
    vlsseg6w.v b10, (brp), bstride
    addi brp, brp, 6*4
    vlsseg6w.v b20, (brp), bstride
    mul t0, nstrip, bstride
    add bp, bp, t0

    mv arp, ap
    mv crp, cp
    li nrows, 6
    
.Lrow_loop:
    vlsseg3w.v aa0, (arp), astride
    addi arp, arp, 3*4

    vfmul.vv cc0, aa0, b00
    vfmul.vv cc1, aa0, b01
    vfmul.vv cc2, aa0, b02
    vfmul.vv cc3, aa0, b03
    vfmul.vv cc4, aa0, b04
    vfmul.vv cc5, aa0, b05

    vfmacc.vv cc0, aa1, b10
    vfmacc.vv cc1, aa1, b11
    vfmacc.vv cc2, aa1, b12
    vfmacc.vv cc3, aa1, b13
    vfmacc.vv cc4, aa1, b14
    vfmacc.vv cc5, aa1, b15
    
    addi nrows, nrows, -1

    vfmacc.vv cc0, aa2, b20
    vfmacc.vv cc1, aa2, b21
    vfmacc.vv cc2, aa2, b22
    vfmacc.vv cc3, aa2, b23
    vfmacc.vv cc4, aa2, b24
    vfmacc.vv cc5, aa2, b25

    vssseg3w.v cc0, (crp), cstride
    addi crp, crp, 6*4

    bnez nrows, .Lrowloop


    mul t0, nstrip, astride
    mul t1, nstrip, cstride
    add ap, ap, t0
    add cp, cp, t1

    sub n, n, nstrip

    bnez n, .Lmatrix_loop

exit:
    ret

