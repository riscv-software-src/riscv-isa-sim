    #
    # kr233 kernel
    # performs stream of <2,3> x <3,3> matrix multiplies
    #  C[2,3] = A[2,3] * B[3,3]
    #
    #               B00 B01 B02
    #               B10 B11 B12
    #               B20 B21 B22
    # 
    #  A00 A01 A02  C00 C01 C02
    #  A10 A11 A12  C10 C11 C12
    #
    # 6 * 4B loads from A
    # 9 * 4B loads from B
    # 6 * 4B stores to C
    #
    # 15*4B loads + 6*4B stores
    # 21 * 4B mem traffic/kernel, 84B/kernel
    #
    # 6 * (3 mul + 2 add) ~ 6 * 3 mulacc = 18 FMAs
    #
    # FLOPS/B = (30/84) = 0.36 FLOPS/B, 1.44 FLOPS/4B (0.86 FMA/4B)
    #
    # >1 4B load or store per FMA
    #
    #  "10,000s" kernels = 840,000 B/run, => MBs working set
    #
    # C prototype
    # void kr233(size_t n, const float* ap, const float* bp, float* cp)
    
#define n a0
#define ap a1
#define bp a2
#define cp a3

#define nstrip a4
#define n24 a5
#define bmp a6
#define bstride a7

/* Straightforward mapping uses 6+6+9 = 21 vector registers. */

#define a00 v0
#define a01 v1
#define a02 v2
#define a10 v3
#define a11 v4
#define a12 v5

#define c00 v8
#define c01 v9
#define c02 v10
#define c10 v11
#define c11 v12
#define c12 v13

#define b00 v16
#define b01 v17
#define b02 v18
#define b10 v19

#define b11 v24
#define b12 v25
#define b20 v26
#define b21 v27
#define b22 v28

    .text
    .global kr233
kr233:

    beqz n, .Lexit

    li bstride, 36          # Use strided segments for B only

.Lstrip:
    vsetvli nstrip, n, e32

    vlsseg4w.v b00, (bp), bstride  # Get b[0][*], and b[1][0]

    addi bmp, bp, 4*4       # Bump intermediate pointer
    slli t0, nstrip, 5      # nstrip * 32
    slli t1, nstrip, 2      # nstrip * 4
    add bp, bp, t0
    add bp, bp, t1          # Bump B pointer, bp += 36*nstrip
    
    vlseg6w.v a00, (ap)     # Bring in a[0-1][0-2]

    sub n, n, nstrip        # Decrement count

    slli t0, nstrip, 4      # nstrip * 16
    slli n24, nstrip, 3     # nstrip * 8
    add n24, n24, t0        # nstrip * 24 bytes
    add ap, ap, n24         # Bump A pointer, ap += 24*nstrip
    
    vlsseg5w.v b12, (bmp), bstride #  Get remaining parts of b[][]

    vfmul.vv c00, a00, b00
    vfmul.vv c10, a10, b00
    vfmul.vv c01, a00, b01
    vfmul.vv c11, a10, b01
    vfmul.vv c02, a00, b02
    vfmul.vv c12, a10, b02

    vfmacc.vv c00, a01, b10
    vfmacc.vv c10, a11, b10

    vfmacc.vv c01, a01, b11
    vfmacc.vv c11, a11, b11
    vfmacc.vv c02, a02, b12
    vfmacc.vv c12, a12, b12

    vfmacc.vv c00, a02, b20
    vfmacc.vv c10, a12, b20
    vfmacc.vv c01, a02, b21
    vfmacc.vv c11, a12, b21
    vfmacc.vv c02, a02, b22
    vfmacc.vv c12, a12, b22

    vsseg6w.v c00, (cp)     # Store results
    add cp, cp, n24         # Bump C pointer, cp += 24*nstrip

    bnez n, .Lstrip

.Lexit:
    ret
