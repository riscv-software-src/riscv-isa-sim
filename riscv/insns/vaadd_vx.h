// vaadd: Averaging adds of integers 
int64_t round = 0;
VRM vrm = STATE.VU.get_vround_mode();
VI_VX_LOOP
({
    int64_t result = rs1 + vs2; 
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
    
    result = result >> 1;

    vd = result; 
})
