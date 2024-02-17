
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "softfloat.h"
#include "math.h"

bfloat16_t ui8_to_bf16(uint8_t input) {
    bfloat16_t result;
    uint16_t exponent = 0, mantissa = 0;

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
        // Find the most significant bit position of the input
        int msbPos = 0;
        for (uint8_t temp = input; temp > 0; temp >>= 1) {
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

    // Combine exponent and mantissa (sign bit is always 0 for unsigned input)
    result.v = exponent | mantissa;

    return result;
}
