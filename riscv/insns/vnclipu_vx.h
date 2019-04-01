// vnclipu: vd[i] = clip(round(vs2[i] + rnd) >> rs1[i])
int64_t round = 0;
VRM vrm = STATE.VU.get_vround_mode();
reg_t sew_lo = 0;
VI_VX_LOOP
({
    if (i % 2 == 0){
        sew_lo = vs2;
        continue;
    }
    
    uint64_t result = (vs2<<sew) | sew_lo;
    // rounding
    switch(vrm){
        case VRM::RUN:
            result += (1 << (sew - 1) );
            break;
        case VRM::RNE:
            assert(true);
            break;
        case VRM::RDN:
            result = (result >> (sew - 1)) << (sew - 1);
            break;
        case VRM::ROD:
            if ((result & (1 << ((sew -1) - 1 ))) > 0){
                result = ((result >> (sew - 1)) + 1) << (sew - 1);
            }
            break;
        case VRM::INVALID_RM:
            assert(true);
    };    
    // unsigned shifting to rs1
    result = result >> rs1;

    // saturation
    if (result >= (uint64_t)(2^(sew - 1)))
        result = (2^(sew - 1)) - 1;
    
    vd = result; 
})
