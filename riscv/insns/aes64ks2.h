
#include "aes_common.h"

require_rv64;
require_extension('K');

uint32_t    rs1_hi  =  RS1 >> 32;
uint32_t    rs2_lo  =  RS2      ;
uint32_t    rs2_hi  =  RS2 >> 32;

uint32_t    r_lo    = (rs1_hi ^ rs2_lo         ) ;
uint32_t    r_hi    = (rs1_hi ^ rs2_lo ^ rs2_hi) ;
uint64_t    result  =  ((uint64_t)r_hi << 32) | r_lo ;

WRITE_RD(result);

