// vkeccak.vi vd, zimm5

#include "zvkned_ext_macros.h"
#include "zvk_ext_macros.h"

#define KECCAK_ROL(data, amt)  (((data) << (amt)) | ((data) >> (64 - (amt))))

// round constants for ι step
const uint64_t KECCAK_RC[25] = {
    0x0000000000000001, // RC[0]	
    0x0000000000008082, // RC[1]	
    0x800000000000808A, // RC[2]	
    0x8000000080008000, // RC[3]	
    0x000000000000808B, // RC[4]	
    0x0000000080000001, // RC[5]	
    0x8000000080008081, // RC[6]	
    0x8000000000008009, // RC[7]	
    0x000000000000008A, // RC[8]	
    0x0000000000000088, // RC[9]	
    0x0000000080008009, // RC[10]
    0x000000008000000A, // RC[11]
    0x000000008000808B, // RC[12]
    0x800000000000008B, // RC[13]
    0x8000000000008089, // RC[14]
    0x8000000000008003, // RC[15]
    0x8000000000008002, // RC[16]
    0x8000000000000080, // RC[17]
    0x000000000000800A, // RC[18]
    0x800000008000000A, // RC[19]
    0x8000000080008081, // RC[20]
    0x8000000000008080, // RC[21] 
    0x0000000080000001, // RC[22]
    0x8000000080008008, // RC[23]
};

require_vkeccak_vi_constraints;

VI_ZVK_VD_VS2_NOOPERANDS_PRELOOP_EGU64x32_NOVM_LOOP(
  {},
  // This statement will be executed before the first execution
  // of the loop, and only if the loop is going to be entered.
  // We cannot use a block ( { ... } ) since we want the variables declared
  // here to be visible in the loop block.
  // We capture the "scalar", vs2's first element, by copy, even though
  // the "no overlap" constraint means that vs2 should remain constant
  // during the loop.
  const uint32_t roundCnt = zimm5;,
  {
    // For VKECCAK
    //  - vd contains the input state,
    //  - vs2 contains the round index,
    //  - vd does receive the output state.
    //
    // note that in the 32 elements of each EGU64x32 only the first 25
    // elements are actually used (the Keccak state is 1600-bit wide)
    EGU64x32_t keccak_state = P.VU.elt_group<EGU64x32_t>(vd_num, idx_eg);

    // unpacking state
    uint64_t A_0_0 = keccak_state[0 + 5 * 0];
    uint64_t A_0_1 = keccak_state[0 + 5 * 1];
    uint64_t A_0_2 = keccak_state[0 + 5 * 2];
    uint64_t A_0_3 = keccak_state[0 + 5 * 3];
    uint64_t A_0_4 = keccak_state[0 + 5 * 4];
    uint64_t A_1_0 = keccak_state[1 + 5 * 0];
    uint64_t A_1_1 = keccak_state[1 + 5 * 1];
    uint64_t A_1_2 = keccak_state[1 + 5 * 2];
    uint64_t A_1_3 = keccak_state[1 + 5 * 3];
    uint64_t A_1_4 = keccak_state[1 + 5 * 4];
    uint64_t A_2_0 = keccak_state[2 + 5 * 0];
    uint64_t A_2_1 = keccak_state[2 + 5 * 1];
    uint64_t A_2_2 = keccak_state[2 + 5 * 2];
    uint64_t A_2_3 = keccak_state[2 + 5 * 3];
    uint64_t A_2_4 = keccak_state[2 + 5 * 4];
    uint64_t A_3_0 = keccak_state[3 + 5 * 0];
    uint64_t A_3_1 = keccak_state[3 + 5 * 1];
    uint64_t A_3_2 = keccak_state[3 + 5 * 2];
    uint64_t A_3_3 = keccak_state[3 + 5 * 3];
    uint64_t A_3_4 = keccak_state[3 + 5 * 4];
    uint64_t A_4_0 = keccak_state[4 + 5 * 0];
    uint64_t A_4_1 = keccak_state[4 + 5 * 1];
    uint64_t A_4_2 = keccak_state[4 + 5 * 2];
    uint64_t A_4_3 = keccak_state[4 + 5 * 3];
    uint64_t A_4_4 = keccak_state[4 + 5 * 4];
    // executed the number of requested keccak rounds
    for (std::size_t ridx = 0; ridx < roundCnt; ++ridx) {
        uint64_t C_0= A_0_0 ^ A_0_1 ^ A_0_2 ^ A_0_3 ^ A_0_4;
        uint64_t C_1= A_1_0 ^ A_1_1 ^ A_1_2 ^ A_1_3 ^ A_1_4;
        uint64_t C_2= A_2_0 ^ A_2_1 ^ A_2_2 ^ A_2_3 ^ A_2_4;
        uint64_t C_3= A_3_0 ^ A_3_1 ^ A_3_2 ^ A_3_3 ^ A_3_4;
        uint64_t C_4= A_4_0 ^ A_4_1 ^ A_4_2 ^ A_4_3 ^ A_4_4;
        uint64_t D_0 = C_4 ^ KECCAK_ROL(C_1,1);
        A_0_0 ^= D_0;
        A_0_1 ^= D_0;
        A_0_2 ^= D_0;
        A_0_3 ^= D_0;
        A_0_4 ^= D_0;
        uint64_t D_1 = C_0 ^ KECCAK_ROL(C_2,1);
        A_1_0 ^= D_1;
        A_1_1 ^= D_1;
        A_1_2 ^= D_1;
        A_1_3 ^= D_1;
        A_1_4 ^= D_1;
        uint64_t D_2 = C_1 ^ KECCAK_ROL(C_3,1);
        A_2_0 ^= D_2;
        A_2_1 ^= D_2;
        A_2_2 ^= D_2;
        A_2_3 ^= D_2;
        A_2_4 ^= D_2;
        uint64_t D_3 = C_2 ^ KECCAK_ROL(C_4,1);
        A_3_0 ^= D_3;
        A_3_1 ^= D_3;
        A_3_2 ^= D_3;
        A_3_3 ^= D_3;
        A_3_4 ^= D_3;
        uint64_t D_4 = C_3 ^ KECCAK_ROL(C_0,1);
        A_4_0 ^= D_4;
        A_4_1 ^= D_4;
        A_4_2 ^= D_4;
        A_4_3 ^= D_4;
        A_4_4 ^= D_4;
        uint64_t T_0 = A_1_0;
        uint64_t T_1 = A_0_2;
        A_0_2 = KECCAK_ROL(T_0, 1);
        uint64_t T_2 = A_2_1;
        A_2_1 = KECCAK_ROL(T_1, 3);
        uint64_t T_3 = A_1_2;
        A_1_2 = KECCAK_ROL(T_2, 6);
        uint64_t T_4 = A_2_3;
        A_2_3 = KECCAK_ROL(T_3, 10);
        uint64_t T_5 = A_3_3;
        A_3_3 = KECCAK_ROL(T_4, 15);
        uint64_t T_6 = A_3_0;
        A_3_0 = KECCAK_ROL(T_5, 21);
        uint64_t T_7 = A_0_1;
        A_0_1 = KECCAK_ROL(T_6, 28);
        uint64_t T_8 = A_1_3;
        A_1_3 = KECCAK_ROL(T_7, 36);
        uint64_t T_9 = A_3_1;
        A_3_1 = KECCAK_ROL(T_8, 45);
        uint64_t T_10 = A_1_4;
        A_1_4 = KECCAK_ROL(T_9, 55);
        uint64_t T_11 = A_4_4;
        A_4_4 = KECCAK_ROL(T_10, 2);
        uint64_t T_12 = A_4_0;
        A_4_0 = KECCAK_ROL(T_11, 14);
        uint64_t T_13 = A_0_3;
        A_0_3 = KECCAK_ROL(T_12, 27);
        uint64_t T_14 = A_3_4;
        A_3_4 = KECCAK_ROL(T_13, 41);
        uint64_t T_15 = A_4_3;
        A_4_3 = KECCAK_ROL(T_14, 56);
        uint64_t T_16 = A_3_2;
        A_3_2 = KECCAK_ROL(T_15, 8);
        uint64_t T_17 = A_2_2;
        A_2_2 = KECCAK_ROL(T_16, 25);
        uint64_t T_18 = A_2_0;
        A_2_0 = KECCAK_ROL(T_17, 43);
        uint64_t T_19 = A_0_4;
        A_0_4 = KECCAK_ROL(T_18, 62);
        uint64_t T_20 = A_4_2;
        A_4_2 = KECCAK_ROL(T_19, 18);
        uint64_t T_21 = A_2_4;
        A_2_4 = KECCAK_ROL(T_20, 39);
        uint64_t T_22 = A_4_1;
        A_4_1 = KECCAK_ROL(T_21, 61);
        uint64_t T_23 = A_1_1;
        A_1_1 = KECCAK_ROL(T_22, 20);
        uint64_t T_24 = A_1_0;
        A_1_0 = KECCAK_ROL(T_23, 44);
        uint64_t C_0_0 = A_0_0;
        uint64_t C_0_1 = A_1_0;
        uint64_t C_0_2 = A_2_0;
        uint64_t C_0_3 = A_3_0;
        uint64_t C_0_4 = A_4_0;
        A_0_0 = C_0_0 ^ (~C_0_1 & C_0_2);
        A_1_0 = C_0_1 ^ (~C_0_2 & C_0_3);
        A_2_0 = C_0_2 ^ (~C_0_3 & C_0_4);
        A_3_0 = C_0_3 ^ (~C_0_4 & C_0_0);
        A_4_0 = C_0_4 ^ (~C_0_0 & C_0_1);
        uint64_t C_1_0 = A_0_1;
        uint64_t C_1_1 = A_1_1;
        uint64_t C_1_2 = A_2_1;
        uint64_t C_1_3 = A_3_1;
        uint64_t C_1_4 = A_4_1;
        A_0_1 = C_1_0 ^ (~C_1_1 & C_1_2);
        A_1_1 = C_1_1 ^ (~C_1_2 & C_1_3);
        A_2_1 = C_1_2 ^ (~C_1_3 & C_1_4);
        A_3_1 = C_1_3 ^ (~C_1_4 & C_1_0);
        A_4_1 = C_1_4 ^ (~C_1_0 & C_1_1);
        uint64_t C_2_0 = A_0_2;
        uint64_t C_2_1 = A_1_2;
        uint64_t C_2_2 = A_2_2;
        uint64_t C_2_3 = A_3_2;
        uint64_t C_2_4 = A_4_2;
        A_0_2 = C_2_0 ^ (~C_2_1 & C_2_2);
        A_1_2 = C_2_1 ^ (~C_2_2 & C_2_3);
        A_2_2 = C_2_2 ^ (~C_2_3 & C_2_4);
        A_3_2 = C_2_3 ^ (~C_2_4 & C_2_0);
        A_4_2 = C_2_4 ^ (~C_2_0 & C_2_1);
        uint64_t C_3_0 = A_0_3;
        uint64_t C_3_1 = A_1_3;
        uint64_t C_3_2 = A_2_3;
        uint64_t C_3_3 = A_3_3;
        uint64_t C_3_4 = A_4_3;
        A_0_3 = C_3_0 ^ (~C_3_1 & C_3_2);
        A_1_3 = C_3_1 ^ (~C_3_2 & C_3_3);
        A_2_3 = C_3_2 ^ (~C_3_3 & C_3_4);
        A_3_3 = C_3_3 ^ (~C_3_4 & C_3_0);
        A_4_3 = C_3_4 ^ (~C_3_0 & C_3_1);
        uint64_t C_4_0 = A_0_4;
        uint64_t C_4_1 = A_1_4;
        uint64_t C_4_2 = A_2_4;
        uint64_t C_4_3 = A_3_4;
        uint64_t C_4_4 = A_4_4;
        A_0_4 = C_4_0 ^ (~C_4_1 & C_4_2);
        A_1_4 = C_4_1 ^ (~C_4_2 & C_4_3);
        A_2_4 = C_4_2 ^ (~C_4_3 & C_4_4);
        A_3_4 = C_4_3 ^ (~C_4_4 & C_4_0);
        A_4_4 = C_4_4 ^ (~C_4_0 & C_4_1);
        /*ι*/ // XL(0,0,RC[i]);
        A_0_0 ^= KECCAK_RC[ridx];
    };
    // epilog: populating back keccak state
    keccak_state[0 + 5 * 0] = A_0_0;
    keccak_state[0 + 5 * 1] = A_0_1;
    keccak_state[0 + 5 * 2] = A_0_2;
    keccak_state[0 + 5 * 3] = A_0_3;
    keccak_state[0 + 5 * 4] = A_0_4;
    keccak_state[1 + 5 * 0] = A_1_0;
    keccak_state[1 + 5 * 1] = A_1_1;
    keccak_state[1 + 5 * 2] = A_1_2;
    keccak_state[1 + 5 * 3] = A_1_3;
    keccak_state[1 + 5 * 4] = A_1_4;
    keccak_state[2 + 5 * 0] = A_2_0;
    keccak_state[2 + 5 * 1] = A_2_1;
    keccak_state[2 + 5 * 2] = A_2_2;
    keccak_state[2 + 5 * 3] = A_2_3;
    keccak_state[2 + 5 * 4] = A_2_4;
    keccak_state[3 + 5 * 0] = A_3_0;
    keccak_state[3 + 5 * 1] = A_3_1;
    keccak_state[3 + 5 * 2] = A_3_2;
    keccak_state[3 + 5 * 3] = A_3_3;
    keccak_state[3 + 5 * 4] = A_3_4;
    keccak_state[4 + 5 * 0] = A_4_0;
    keccak_state[4 + 5 * 1] = A_4_1;
    keccak_state[4 + 5 * 2] = A_4_2;
    keccak_state[4 + 5 * 3] = A_4_3;
    keccak_state[4 + 5 * 4] = A_4_4;

    // Update the destination register.
    EGU64x32_t &vd = P.VU.elt_group<EGU64x32_t>(vd_num, idx_eg, true);
    EGU64x32_COPY(vd, keccak_state);
  }
);
