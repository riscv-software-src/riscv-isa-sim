
require_extension(EXT_ZKSED);

#include "sm4_common.h"

uint8_t  bs     = insn.bs();

uint32_t sb_in  = (RS2 >> (8*bs)) & 0xFF;
uint32_t sb_out = sm4_sbox[sb_in];

uint32_t x      = sb_out ^
    ((sb_out & 0x07) << 29) ^ ((sb_out & 0xFE) <<  7) ^
    ((sb_out & 0x01) << 23) ^ ((sb_out & 0xF8) << 13) ;

uint32_t rotl   = (x << (8*bs)) | (x >> (32-8*bs));

uint32_t result = rotl ^ RS1;

WRITE_RD(sext32(result));

