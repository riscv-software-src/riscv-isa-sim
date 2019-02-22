#
#   Function prototype
#   void kr33inv(size_t n, const float *a, float*c)
#
# With some software pipelining

#define n a0
#define ap a1
#define cp a2
#define nstrip a3
#define mstride a4
#define mfour a5
#define arp a6
#define crp a7
#define stripstride t6

#define fone f31

#define ia00   v9
#define	ia01   v10
#define	ia02   v11
#define	ia10   v12
#define	ia11   v13
#define	ia12   v14
#define	ia20   v15
#define	ia21   v16
#define	ia22   v17
#define deta   v30
#define ideta  v31

    .text
    .global k33inv
kr33inv:
    li mstride, 3*3*4   # Stride between matrices
    li mfour, 4*4       # Step across four elements 
    li t0, FLOATONE     # floating-point 1.0f
    fmv t0, fone        # Set fone to 1.0f

    vsetvli x0, n, e32
    add arp, ap, mfour
    vlsseg5w.v v4, (arp), mstride

.Lloop:
    vsetvli nstrip, n, e32
    
    vlsseg4w.v v0, (ap), mstride
    mul stripstride, nstrip, mstride
    add ap, ap, stripstride

    vfmul.vv ia00, v4, v8
	vfmul.vv ia10, v6, v5
    vfnmsac.vv ia00, v5, v7

    sub n, n, nstrip

    vfmul.vv ia20, v3, v7
	vfnmsac.vv ia10, v3, v8
	vfnmsac.vv ia20, v4, v6

    vfmul.vv  deta, ia00, v0
    vfmacc.vv deta, ia10, v1
    vfmacc.vv deta, ia20, v2

    vfrdiv.vf ideta, deta, fone

	vfmul.vv ia01, v7, v2
	vfmul.vv ia12, v3, v2
	vfmul.vv ia02, v5, v1
	vfmul.vv ia21, v6, v1

    vfnmsac.vv ia11, v2, v6
	vfnmsac.vv ia01, v8, v1

    add arp, ap, mfour
	vfnmsac.vv ia12, v0, v5
	vfnmsac.vv ia02, v2, v4
	vfnmsac.vv ia21, v7, v0

    vsetvli  x0, n, e32             # Prefetch next a_ref value, if any
    vlsseg5w.v v4, (arp), mstride   
    vsetvli x0, nstrip, e32         # Restore current vl

    vfnmsac.vv ia22, v1, v3

    vfmul.vv ia00, ia00, ideta
    vfmul.vv ia01, ia01, ideta
    vfmul.vv ia02, ia02, ideta
    vfmul.vv ia10, ia10, ideta

    vfmul.vv ia11, ia11, ideta

    vssseg4w.v ia00, (cp), mstride # Save first four values from matrices
    add crp, cp, mfour

    vfmul.vv ia12, ia12, ideta
    vfmul.vv ia20, ia20, ideta
    vfmul.vv ia21, ia21, ideta
    vfmul.vv ia22, ia22, ideta

    vssseg5w.v ia11, (crp), mstride # Save remaining five values
    add cp, cp, stripstride

    bnez n, .Lloop

exit:
    ret

