// vaesef.vv vd, vs2

#include "zvkned_ext_macros.h"
#include "zvk_ext_macros.h"

require_vaes_vv_constraints;

VI_ZVK_VD_VS2_NOOPERANDS_PRELOOP_EGU32x4_NOVM_LOOP(
  {},
  {},  // No PRELOOP.
  {
    // For AES128, AES192, or AES256, state and key are 128b/16B values:
    //  - vd contains the input state,
    //  - vs2 contains the round key,
    //  - vd receives the output state.
    //
    // While the spec calls for handling the vector as made of EGU32x4
    // element groups (i.e., 4 uint32_t), it is convenient to treat
    // AES state and key as EGU8x16 (i.e., 16 uint8_t). This is why
    // we extract the operands here instead of using the existing LOOP
    // macro that defines/extracts the operand variables as EGU32x4.
    EGU8x16_t aes_state = P.VU.elt_group<EGU8x16_t>(vd_num, idx_eg);
    const EGU8x16_t round_key = P.VU.elt_group<EGU8x16_t>(vs2_num, idx_eg);

    // SubBytes - Apply S-box to every byte in the state
    VAES_SUB_BYTES(aes_state);
    // ShiftRows - Rotate each row bytes by 0, 1, 2, 3 positions.
    VAES_SHIFT_ROWS(aes_state);
    // MixColumns is not performed for the final round.
    // AddRoundKey
    EGU8x16_XOREQ(aes_state, round_key);

    // Update the destination register.
    EGU8x16_t &vd = P.VU.elt_group<EGU8x16_t>(vd_num, idx_eg, true);
    EGU8x16_COPY(vd, aes_state);
  }
);
