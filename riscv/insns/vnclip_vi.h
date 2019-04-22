// vnclip: vd[i] = clip(round(vs2[i] + rnd) >> simm)
int64_t round = 0;
VRM vrm = STATE.VU.get_vround_mode();
reg_t sew_lo = 0;
VI_VI_LOOP
({
    if (i % 2 == 0){
        sew_lo = vs2;
        continue;
    }
    
    int64_t result = (vs2<<sew) | sew_lo;
    // rounding
    switch(vrm){
        case VRM::RNU:
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
    result = result >> simm5;

    // saturation
    if (result >= (int64_t)(2^(sew - 1)))
        result = (2^(sew - 1)) - 1;
    
	vd = result; 
})
