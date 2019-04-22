// vnclip: vd[i] = clip(round(vs2[i] + rnd) >> vs1[i])
VRM xrm = STATE.VU.get_vround_mode();
VI_VV_LOOP
({
    int64_t result = vs2; // expend the vs2 size more than SEW
    int64_t old = result; 
    int64_t sign_mask = 1 << (sew - 1);
    // rounding
    INT_ROUNDING(result, xrm, sew);

    // saturation
    if ((old & sign_mask) != (result & sign_mask)){
       result = result >> 1; 
       STATE.VU.vxsat = 1;
    }
    // unsigned shifting to rs1
    result = result >> vs1;


    vd = result;
})
