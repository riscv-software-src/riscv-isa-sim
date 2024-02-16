
#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "softfloat.h"

float8_2_t f8_2_sub( float8_2_t a, float8_2_t b )
{
    union ui8_f8_2 uA;
    uint_fast8_t uiA;
    union ui8_f8_2 uB;
    uint_fast8_t uiB;
#if ! defined INLINE_LEVEL || (INLINE_LEVEL < 1)
    float8_2_t (*magsFuncPtr)( uint_fast8_t, uint_fast8_t );
#endif

    uA.f = a;
    uiA = uA.ui;
    uB.f = b;
    uiB = uB.ui;
#if defined INLINE_LEVEL && (1 <= INLINE_LEVEL)
    if ( signF8_2UI( uiA ^ uiB ) ) {
        return softfloat_addMagsF8_2( uiA, uiB );
    } else {
        return softfloat_subMagsF8_2( uiA, uiB );
    }
#else
    magsFuncPtr =
        signF8_2UI( uiA ^ uiB ) ? softfloat_addMagsF8_2 : softfloat_subMagsF8_2;
    return (*magsFuncPtr)( uiA, uiB );
#endif

}

