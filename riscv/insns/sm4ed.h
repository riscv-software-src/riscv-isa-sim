
require_extension('K');

#include "sm4_common.h"

uint8_t  bs     = insn.bs();

uint32_t sb_in  = (RS2 >> (8*bs)) & 0xFF;
uint32_t sb_out = (uint32_t)sm4_sbox[sb_in];

uint32_t linear = sb_out ^  (sb_out         <<  8) ^ 
                            (sb_out         <<  2) ^
                            (sb_out         << 18) ^
                           ((sb_out & 0x3f) << 26) ^
                           ((sb_out & 0xC0) << 10) ;

uint32_t rotl   = (linear << (8*bs)) | (linear >> (32-8*bs));

uint32_t result = rotl ^ RS1;

WRITE_RD(sext32(result));

