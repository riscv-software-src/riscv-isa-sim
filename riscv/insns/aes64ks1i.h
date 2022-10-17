
#include "aes_common.h"

require_rv64;
require_either_extension(EXT_ZKND, EXT_ZKNE);

uint8_t     round_consts [10] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

uint8_t     enc_rcon          = insn.rcon() ;

require(enc_rcon <= 0xA);

uint32_t    temp              = (RS1 >> 32) & 0xFFFFFFFF  ;
uint8_t     rcon              = 0            ;
uint64_t    result                           ;

if (enc_rcon != 0xA) {
    temp    = (temp >> 8) | (temp << 24); // Rotate right by 8
    rcon    = round_consts[enc_rcon];
}

temp        =
    ((uint32_t)AES_ENC_SBOX[(temp >> 24) & 0xFF] << 24) |
    ((uint32_t)AES_ENC_SBOX[(temp >> 16) & 0xFF] << 16) |
    ((uint32_t)AES_ENC_SBOX[(temp >>  8) & 0xFF] <<  8) |
    ((uint32_t)AES_ENC_SBOX[(temp >>  0) & 0xFF] <<  0) ;

temp       ^= rcon;

result      = ((uint64_t)temp << 32) | temp;

WRITE_RD(result);

