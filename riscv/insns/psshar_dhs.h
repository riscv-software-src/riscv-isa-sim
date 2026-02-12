require_rv32;
P_RD_RS1_DW_LOOP(16, 16, {
    uint64_t bits_SMIN = (uint64_t{1} << (16 - 1));
    uint64_t bits_SMAX = ((uint64_t{1} << (16 - 1)) - 1);
    bool ov = false;
    int8_t m = P_FIELD(RS2, 0, 8);
    int8_t  rev = static_cast<int8_t>(m);
    rev = (m < 0) ? static_cast<uint8_t>(~m + 1u) : m;
    uint64_t mask = ((uint64_t{1} << 16) - 1);
    p_rs1 &= mask;
    if(m < 0){
        if ((rev & 0xFFu) == 0u) 
            p_rd = (uint16_t)p_rs1;
        else{  
            __int128 v_sext;
            bool neg = ((p_rs1 >> (16 - 1)) & 1u);
            if(!neg) v_sext = static_cast<__int128>(p_rs1);
            else v_sext = (static_cast<__int128>(-1) << 16) | static_cast<__int128>(p_rs1);
            __int128 v_cat0 = v_sext << 1;

            unsigned sh = (rev < 0) ? 0u : (unsigned)rev;

            __int128 sra_val;
            if(sh == 0) 
                sra_val = v_cat0;
            else if(sh >=127)
                sra_val = (v_cat0 < 0) ? static_cast<__int128>(-1) : static_cast<__int128>(0);
            else{
                __int128 ux = static_cast<unsigned __int128>(v_cat0);
                __int128 shifted = ux >> sh;
                if(v_cat0 < 0)
                    shifted |= (~static_cast<unsigned __int128>(0)) << (128 - sh);
                sra_val = static_cast<__int128>(shifted);
            }

            __int128 plus1 = sra_val + static_cast<__int128>(1);
            unsigned __int128 ures = static_cast<unsigned __int128>(plus1);
            p_rd = (uint16_t)(static_cast<uint64_t>((ures >> 1) & static_cast<unsigned __int128>(mask)));
        }
    }
    else{
        if(rev==0) p_rd = (uint16_t)p_rs1;
        else if(rev >= 16){
            if(p_rs1==0) 
                p_rd = 0;
            else{
                ov = true;
                uint64_t sign = (p_rs1 >> (16 - 1)) & 1u;
                p_rd = (uint16_t)(sign ? bits_SMIN : bits_SMAX);
            }
        }
        else{
            uint64_t sign = (p_rs1 >> (16 - 1)) & 1u;
            uint64_t top  = (p_rs1 >> (16 - rev));
            uint64_t need = sign ? ((uint64_t{1} << rev) - 1) : 0u;
            ov = (top != need);
            if(ov)
                p_rd = (uint16_t)(sign ? bits_SMIN : bits_SMAX);
            else
                p_rd =  (uint16_t)((p_rs1 << rev) & mask);
        }
    }
})
