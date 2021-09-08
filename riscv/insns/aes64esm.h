
#include "aes_common.h"

require_rv64;
require_extension(EXT_ZKNE);

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

