// vfredsum.vs  vd, vs2, vs1, vm # Unordered sum
{	VI_CHECK_REDUCTION(false)
	VI_VFP_COMMON
    reg_t tmp_vl = ((vl>64)?128:(vl>32)?64:(vl>16)?32:(vl>8)?16:(vl>4)?8:(vl>2)?4:(vl>1)?2:1);
    float32_t tmpValue[tmp_vl];
    float32_t tmpZero;
	tmpZero.v = 0;
    reg_t i= 0;
    while(i<P.VU.vstart){
        tmpValue[i] =  tmpZero;
        ++i;
    }
    for (i=P.VU.vstart; i<tmp_vl; ++i){
        const int mlen = P.VU.vmlen;
        const int midx = (mlen * i) / 32;
        const int mpos = (mlen * i) % 32;
        bool skip = ((P.VU.elt<uint32_t>(0, midx) >> mpos) & 0x1) == 0;
        if ((insn.v_vm() == 0 && skip)|| i>=vl)
            tmpValue[i].v =  0;
        else
            tmpValue[i] = P.VU.elt<float32_t>(rs2_num, i);
    }
    while(tmp_vl >4){
        for (reg_t i=0; i< tmp_vl/2;i=i+4){
            tmpValue[i+0] = f32_add(tmpValue[(i*2)+0], tmpValue[(i*2)+0+4]);
            set_fp_exceptions;
            tmpValue[i+1] = f32_add(tmpValue[(i*2)+1], tmpValue[(i*2)+1+4]);
            set_fp_exceptions;
            tmpValue[i+2] = f32_add(tmpValue[(i*2)+2], tmpValue[(i*2)+2+4]);
            set_fp_exceptions;
            tmpValue[i+3] = f32_add(tmpValue[(i*2)+3], tmpValue[(i*2)+3+4]);
            set_fp_exceptions;
        }
        tmp_vl /=2;
    }
    if(tmp_vl>2){
        tmpValue[0] = f32_add(tmpValue[0], tmpValue[2]);
        set_fp_exceptions;
        tmpValue[1] = f32_add(tmpValue[1], tmpValue[3]);
        set_fp_exceptions;
        tmp_vl /=2;
    }
    if(tmp_vl>1){
        tmpValue[0] = f32_add(tmpValue[0], tmpValue[1]);
        set_fp_exceptions;
    }
    P.VU.vstart = 0;
    if (vl > 0) {
        float32_t vd_0 = P.VU.elt<float32_t>(rs1_num, 0);
        vd_0 = f32_add(vd_0, tmpValue[0]);
        set_fp_exceptions;
        P.VU.elt<type_sew_t<e32>::type>(rd_num, 0, true) = vd_0.v;
    }
}
