
#include "aes_common.h"

require_rv32;
require_extension('K');
require(RD == 0); // Additional decoding required for RV32

uint8_t     bs = insn.bs();

uint8_t     t0 = RS2 >> (8*bs);
uint8_t      x = AES_DEC_SBOX[t0];
uint32_t     u = x;

u = (u << (8*bs)) | (u >> (32-8*bs));

uint64_t    rd = insn.rs1(); // RD sourced from RS1 field.
WRITE_REG(rd, u ^ RS1);

