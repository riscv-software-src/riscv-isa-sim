// vfredosum: vd[0] =  sum( vs2[*] , vs1[0]  )
softfloat_roundingMode = STATE.VU.vxrm;
float32_t sum;
float32_t vs1_0;
VFP_VV_LOOP
({
    if(i == 0){
        vs1_0 = vs1;
    }
    switch(STATE.VU.vsew){
    case e32:
       sum = f32_add(sum, f32_add(vs2, vs1_0));
       break;
    case e16:
    case e8:
    default:
        softfloat_exceptionFlags = 1;
    };
    vd = sum;
})
