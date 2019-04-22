// vsmul
int64_t round = 0;
VRM vrm = STATE.VU.get_vround_mode();
VI_VX_LOOP
({
    int64_t result = vsext(rs1 * vs2, sew * 2);
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
    
    // saturation
    if (result >= (int64_t)(2^(sew - 1)))
        result = (2^(sew - 1)) - 1;
    
})
