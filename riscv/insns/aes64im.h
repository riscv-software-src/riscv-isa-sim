
#include "aes_common.h"

require_rv64;
require_extension(EXT_ZKND);

uint32_t col_0 = RS1 & 0xFFFFFFFF;
uint32_t col_1 = RS1 >> 32       ;

         col_0 = AES_INVMIXCOLUMN(col_0);
         col_1 = AES_INVMIXCOLUMN(col_1);

uint64_t result= ((uint64_t)col_1 << 32) | col_0;

WRITE_RD(result);

