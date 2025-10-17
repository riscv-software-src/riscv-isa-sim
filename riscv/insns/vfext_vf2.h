static const uint8_t ofp4_to_e4m3[16] = {
    0x00, 0x30, 0x38, 0x3c, 0x40, 0x44, 0x48, 0x4c,  // positive values (sign bit 0)
    0x80, 0xb0, 0xb8, 0xbc, 0xc0, 0xc4, 0xc8, 0xcc   // negative values (sign bit 1)
};

require_extension(EXT_ZVFOFP4MIN);
VI_NON_ALTFMT_INSN
VI_VF_EXT(2,
    {
        uint_fast8_t packed_ofp4_reg = P.VU.elt<uint8_t>(rs2_num, i / 2);
        uint_fast8_t data = ((packed_ofp4_reg >> ((i & 1UL)*4)) & 0xF);
        P.VU.elt<uint8_t>(rd_num, i, true) = ofp4_to_e4m3[data];
    }
)
