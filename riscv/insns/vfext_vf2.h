static const uint8_t ofp4_to_e4m3_lower[8] = {
    0x00, 0x30, 0x38, 0x3c, 0x40, 0x44, 0x48, 0x4c
};

require_extension(EXT_ZVFOFP4MIN);
require(!P.VU.altfmt);
VI_VF_EXT(2,
    {
        uint_fast8_t packed_ofp4_reg = P.VU.elt<uint8_t>(rs2_num, i / 2);
        uint_fast8_t data = ((packed_ofp4_reg >> ((i & 1UL)*4)) & 0xF);
        P.VU.elt<uint8_t>(rd_num, i, true) = (ofp4_to_e4m3_lower[data& 0x7] | ((data & 0x8) << 4));
    }
)
