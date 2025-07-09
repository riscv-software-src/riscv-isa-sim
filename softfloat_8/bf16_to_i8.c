
#include <stdint.h>
#include "specialize.h"
#include "softfloat.h"

#include <math.h>

int round_i8(int sign, int result, int precision, int inexact);

int8_t bf16_to_i8(bfloat16_t input, uint_fast8_t roundingMode) {
    // Extract sign, exponent, and mantissa from the bfloat16 input
    int16_t sign = (input.v & 0x8000) ? -1 : 1;
    int16_t exponent = ((input.v >> 7) & 0xFF); // exponent
    int16_t normalized = (exponent != 0); // Normalized Checking
    exponent -= 127; // De-bias the exponent
    int16_t implicit_bit = normalized << 7;
    uint16_t mantissa_dec = (input.v & 0x7F) | implicit_bit; // Add the implicit leading bit
    double mantissa = 0;
    // mantissa extraction
    for (int i=7;i>=0;i--) { // 8 iterations on 7 mantissa bits going from 7 down to 0 because we consider also the implicit bit 
        int16_t extract_bit = (mantissa_dec >> i) & 0x1; // extract the bit of the ith index
        if (extract_bit) { // if the bit is '1' we accumulate to the mantissa
           mantissa = mantissa + pow(2, i-7); // the value accumulated to the mantissa equals 2^(i-7)
        }
    }
    // Calculate the floating-point result represented by the bfloat16
    // Since bfloat16 has limited precision, and we're converting to int8, simplifications are made
    double result_abs = mantissa * pow(2, exponent); // absolute  result
    double result = sign * result_abs;

    // Raise the invalid flag to handle out of range
    if (result > INT8_MAX) {
        softfloat_raiseFlags(softfloat_flag_invalid);
        return INT8_MAX;
    }
    else if (result < INT8_MIN) {
        softfloat_raiseFlags(softfloat_flag_invalid);
        if (input.v > 0xFF80) { // input is a -NaN, then we saturate to the largest positive number
            return INT8_MAX;
        }
        else {
            return INT8_MIN;
        }
    }

    int inexact = 0;
    int precision = 0;

    if (result != floor(result)) { // if this statement is true, then the result is not a whole number and rounding is needed
        softfloat_raiseFlags(softfloat_flag_inexact); // raise the inexact flag
        if ((softfloat_exceptionFlags & 0x1) == 1) {
            if (result == (floor(result) + ceil(result))/2) { // right in the middle
                inexact   = 0;
                precision = 1;
            }
            else if (fabs(result) < fabs((floor(result) + ceil(result))/2)) { // below the middle
                inexact   = 1;
                precision = 0;
            }
            else { // above the middle
                inexact   = 1;
                precision = 1;
            }
        }
    }
    return (int8_t)result + round_i8(sign, result, precision, inexact);
}


int round_i8(int sign, int result, int precision, int inexact) {
    //printf("precision = %d, inexact = %x\n", precision, inexact);
    int round = 0; // default
    if (softfloat_roundingMode == 0) { // RNE
        int even_mask = 0x1; // used to mask the result to see if the number is even (i.e. LSB = 0)
        if (inexact == 0) {
            if (precision == 1) { // in the middle
                if (result & even_mask == 1) { // if the even number is above the middle
                    round = sign * (precision | inexact); // round to the absolute highest
                }
            }
        }
        else { // above or below
            if (precision == 1) { // if above
                round = sign * (precision | inexact); // round to the absolute highest
            }
        }
    }
    else if (softfloat_roundingMode == 1) { // RTZ
        round = 0;
    }
    else if (softfloat_roundingMode == 2) { // RDN
        if (sign == 1) {
            round = 0;
        }
        else {
            round = sign * (precision | inexact);
        }
    }
    else if (softfloat_roundingMode == 3) { // RUP
        if (sign == -1) {
            round = 0;
        }
        else {
            round = sign * (precision | inexact);
        }
    }
    else if (softfloat_roundingMode == 4) { // RMM
        if (precision == 1) {
            round = sign;
        }
        else {
            round = 0;
        }
    }
    return round;
}
