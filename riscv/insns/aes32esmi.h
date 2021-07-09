
#include "aes_common.h"

require_rv32;
require_extension('K');

uint8_t     bs = insn.bs();

uint8_t     t0 = RS2 >> (8*bs);
uint8_t      x = AES_ENC_SBOX[t0];
uint32_t     u ;

u = (AES_GFMUL(x,3) << 24) |
    (          x    << 16) |
    (          x    <<  8) |
    (AES_GFMUL(x,2) <<  0) ;

u = (u << (8*bs)) | (u >> (32-8*bs));

WRITE_RD(sext_xlen(u ^ RS1));

