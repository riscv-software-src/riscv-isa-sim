#
#   Function prototype
#   void kr33inv(size_t n, const float *a, float*c)
#
# With unrolling and software pipelining

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

# v0-v9 holds first unroll input matrix
# nv0-nv9 holds second unroll input matrix

#define ia00   v9
#define	ia01   v10
#define	ia02   v11
#define	ia10   v12
#define	ia11   v13
#define	ia12   v14
#define	ia20   v15
#define	ia21   v16
#define	ia22   v17

#define nv0    v20
#define nv1    v21
#define nv2    v22
#define nv3    v23
#define nv4    v24
#define nv5    v25
#define nv6    v26
#define nv7    v27
#define nv8    v28
#define nv9    v29

#define deta   v30
#define ideta  v31

#define FLOATONE 0x3f800000

    .text
    .global k33inv
kr33inv:
    li mstride, 3*3*4   # Stride between matrices
    li mfour, 4*4       # Step across four elements 
    li t0, FLOATONE     # floating-point 1.0f
    fmv.w.x fone, t0        # Set fone to 1.0f

    vsetvli nstrip, n, e32
    add arp, ap, mfour
    vlsseg5w.v v4, (arp), mstride

    vlsseg4w.v v0, (ap), mstride
    mul stripstride, nstrip, mstride
    add ap, ap, stripstride

.Lloop:
    vsetvli nstrip, n, e32

    vfmul.vv ia00, v4, v8
	vfmul.vv ia10, v6, v5
    vfnmsac.vv ia00, v5, v7
	vfnmsac.vv ia10, v3, v8

    sub n, n, nstrip
    add arp, ap, mfour

    # Push scheduling of ia20 down to reduce WAR hazard wtih vssseg

    vfmul.vv ia20, v3, v7
    vfmul.vv deta, ia00, v0
	vfnmsac.vv ia20, v4, v6
    vfmacc.vv deta, ia10, v1
    vfmacc.vv deta, ia20, v2

    vsetvli  x0, n, e32             # Prefetch next a_ref value, if any
    vlsseg5w.v nv4, (arp), mstride   
    vsetvli x0, nstrip, e32         # Restore current vl

    vfrdiv.vf ideta, deta, fone

	vfmul.vv ia01, v7, v2
	vfmul.vv ia12, v3, v2
	vfmul.vv ia02, v5, v1
	vfmul.vv ia21, v6, v1

    vfnmsac.vv ia11, v2, v6
	vfnmsac.vv ia01, v8, v1
    vfnmsac.vv ia12, v0, v5

    vsetvli  x0, n, e32             # Prefetch next a_ref value, if any
    vlsseg4w.v nv0, (ap), mstride
    vsetvli x0, nstrip, e32         # Restore current vl

	vfnmsac.vv ia02, v2, v4
	vfnmsac.vv ia21, v7, v0
    vfnmsac.vv ia22, v1, v3

    mul stripstride, nstrip, mstride
    add ap, ap, stripstride

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

    vsetvli nstrip, n, e32    # Second vector length in loop.

    vfmul.vv ia00, nv4, nv8
	vfmul.vv ia10, nv6, nv5
    vfnmsac.vv ia00, nv5, nv7
	vfnmsac.vv ia10, nv3, nv8

    sub n, n, nstrip
    add arp, ap, mfour

    # Push scheduling of ia20 down to reduce WAR hazard wtih vssseg

    vfmul.vv ia20, nv3, nv7
    vfmul.vv deta, ia00, nv0
	vfnmsac.vv ia20, nv4, nv6
    vfmacc.vv deta, ia10, nv1
    vfmacc.vv deta, ia20, nv2

    vsetvli  x0, n, e32             # Prefetch next a_ref value, if any
    vlsseg5w.v v4, (arp), mstride   
    vsetvli x0, nstrip, e32         # Restore current vl

    vfrdiv.vf ideta, deta, fone

	vfmul.vv ia01, nv7, nv2
	vfmul.vv ia12, nv3, nv2
	vfmul.vv ia02, nv5, nv1
	vfmul.vv ia21, nv6, nv1

    vfnmsac.vv ia11, nv2, nv6
	vfnmsac.vv ia01, nv8, nv1
    vfnmsac.vv ia12, nv0, nv5

    vsetvli  x0, n, e32             # Prefetch next a_ref value, if any
    vlsseg4w.v v0, (ap), mstride
    vsetvli x0, nstrip, e32         # Restore current vl

	vfnmsac.vv ia02, nv2, nv4
	vfnmsac.vv ia21, nv7, nv0
    vfnmsac.vv ia22, nv1, nv3

    mul stripstride, nstrip, mstride
    add ap, ap, stripstride

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

