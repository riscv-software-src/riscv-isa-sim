require_rv32;
P_RD_RS1_DW_LOOP(32, 32, {
    uint64_t bits_SMIN = (uint64_t{1} << (32 - 1));
    uint64_t bits_SMAX = ((uint64_t{1} << (32 - 1)) - 1);
    bool ov = false;
    int8_t m = P_FIELD(RS2, 0, 8);
    int8_t  rev = static_cast<int8_t>(m);
    rev = (m < 0) ? static_cast<uint8_t>(~m + 1u) : m;
    uint64_t mask = ((uint64_t{1} << 32) - 1);
    p_rs1 &= mask;
    if(m < 0){
        if ((rev & 0xFFu) == 0u) 
            p_rd = (uint32_t)p_rs1;
        else{  
            int128_t v_sext;
            bool neg = ((p_rs1 >> (32 - 1)) & 1u);
            if(!neg) v_sext = static_cast<int128_t>(p_rs1);
            else v_sext = static_cast<int128_t>((~static_cast<uint128_t>(0) << 32) | static_cast<uint128_t>(p_rs1));
            int128_t v_cat0 = v_sext << 1;

            unsigned sh = ((unsigned)(uint8_t)rev > 255u) ? 255u : (unsigned)(uint8_t)rev;

            int128_t sra_val;
            if(sh == 0)
                sra_val = v_cat0;
            else if(sh >=127)
                sra_val = (v_cat0 < 0) ? static_cast<int128_t>(-1) : static_cast<int128_t>(0);
            else{
                int128_t ux = static_cast<uint128_t>(v_cat0);
                int128_t shifted = ux >> sh;
                if(v_cat0 < 0)
                    shifted |= (~static_cast<uint128_t>(0)) << (128 - sh);
                sra_val = static_cast<int128_t>(shifted);
            }

            int128_t plus1 = sra_val + static_cast<int128_t>(1);
            uint128_t ures = static_cast<uint128_t>(plus1);
            p_rd = (uint32_t)(static_cast<uint64_t>((ures >> 1) & static_cast<uint128_t>(mask)));
        }
    }
    else{
        if(rev==0) p_rd = (uint32_t)p_rs1;
        else if(rev >= 32){
            if(p_rs1==0)
                p_rd = 0;
            else{
                ov = true;
                uint64_t sign = (p_rs1 >> (32 - 1)) & 1u;
                p_rd = (uint32_t)(sign ? bits_SMIN : bits_SMAX);
            }
        }
        else{
            uint64_t sign = (p_rs1 >> (32 - 1)) & 1u;
            uint64_t top  = (p_rs1 >> (32 - rev));
            uint64_t need = sign ? ((uint64_t{1} << rev) - 1) : 0u;
            ov = (top != need);
            if(ov)
                p_rd = (uint32_t)(sign ? bits_SMIN : bits_SMAX);
            else
                p_rd =  (uint32_t)((p_rs1 << rev) & mask);
        }
    }
  if (ov) P.VU.vxsat->write(1);
})
