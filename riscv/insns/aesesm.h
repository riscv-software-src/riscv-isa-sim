
#include "aes_common.h"

// The encodings for the RV32 and RV64 AES instructions overlap, as they
// are mutually exclusive. They have rather different functionality.

if(xlen == 32) {
    // Execute the RV32 aes32esmi instruction

    require_rv32;
    require_extension('K');
    require(RD == 0); // Additional decoding required for RV32

    uint8_t     bs = insn.bs();

    uint8_t     t0 = RS2 >> (8*bs);
    uint8_t      x = AES_ENC_SBOX[t0];
    uint32_t     u ;

    u = (AES_GFMUL(x,3) << 24) |
        (          x    << 16) |
        (          x    <<  8) |
        (AES_GFMUL(x,2) <<  0) ;

    u = (u << (8*bs)) | (u >> (32-8*bs));

    uint64_t    rd = insn.rs1(); // RD sourced from RS1 field.
    WRITE_REG(rd, u ^ RS1);

} else {
    // Execute the RV64 aes64esm instruction

    require(insn.bs() == 0);
    require_rv64;
    require_extension('K');

    uint64_t temp = AES_SHIFROWS_LO(RS1,RS2);

             temp = (
        ((uint64_t)AES_ENC_SBOX[(temp >>  0) & 0xFF] <<  0) |
        ((uint64_t)AES_ENC_SBOX[(temp >>  8) & 0xFF] <<  8) |
        ((uint64_t)AES_ENC_SBOX[(temp >> 16) & 0xFF] << 16) |
        ((uint64_t)AES_ENC_SBOX[(temp >> 24) & 0xFF] << 24) |
        ((uint64_t)AES_ENC_SBOX[(temp >> 32) & 0xFF] << 32) |
        ((uint64_t)AES_ENC_SBOX[(temp >> 40) & 0xFF] << 40) |
        ((uint64_t)AES_ENC_SBOX[(temp >> 48) & 0xFF] << 48) |
        ((uint64_t)AES_ENC_SBOX[(temp >> 56) & 0xFF] << 56) 
    );

    uint32_t col_0 = temp & 0xFFFFFFFF;
    uint32_t col_1 = temp >> 32       ;

             col_0 = AES_MIXCOLUMN(col_0);
             col_1 = AES_MIXCOLUMN(col_1);

    uint64_t result= ((uint64_t)col_1 << 32) | col_0;

    WRITE_RD(result);

}

