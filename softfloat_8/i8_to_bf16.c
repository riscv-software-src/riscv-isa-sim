
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "softfloat.h"
#include "math.h"


bfloat16_t i8_to_bf16(int8_t input) {
    bfloat16_t result;
    uint16_t sign = 0, exponent = 0, mantissa = 0;

    if (input == -128) {
        // For -128, manually set to the equivalent positive value in bfloat
        sign = 0x8000; // Set the sign bit if negative
        exponent = 0x86 << 7; // Set the exponet
        mantissa = 0; // Set the mantissa
        goto END;
    } else if (input < 0) { // Handle sign 
        sign = 0x8000; // Set the sign bit if negative
        input = -input; // Work with positive value for simplicity
    }

    if (input == 0) {
        // If input is 0, the result is simply 0
        result.v = 0;
        return result;
    } else if (input == 1) {
        // Special case for 1, to ensure it gets a distinct representation
        exponent = (127 << 7); // Exponent for 1 in floating-point representation
        mantissa = 0; // Mantissa is all zeros for 1
    } else {
        // For input > 1, calculate exponent and mantissa
        // This simplified approach increments the exponent based on the input value
        // and sets the mantissa to a representation of the input

        // Find the most significant bit position of the input
        int msbPos = 0;
        for (int8_t temp = input; temp > 0; temp >>= 1) {
            msbPos++;
        }

        // Calculate exponent
        // Bias of 127 plus position of MSB minus one for the implicit leading bit in floating-point representation
        exponent = ((127 + msbPos - 1) << 7) & 0xFF80; 

        // Calculate mantissa
        // Shift input left to align MSB with the 8th position (7 bits for mantissa)
        // Then take the upper 7 bits as the mantissa
        int shiftLeft = 7 - (msbPos - 1);
        mantissa = (input << shiftLeft) & 0x7F; // Mask to get the top 7 bits
    }

    END:
    // Combine sign, exponent, and mantissa
    result.v = sign | exponent | mantissa;

    return result;
}
