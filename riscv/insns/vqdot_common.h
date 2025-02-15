#define UNPACK_32_TO_8(IN, TYPE, OUT_ARR) \
    int64_t OUT_ARR[4]; \
    OUT_ARR[0] = (TYPE)((IN) & 0xff); \
    OUT_ARR[1] = (TYPE)((IN >> 8) & 0xff); \
    OUT_ARR[2] = (TYPE)((IN >> 16) & 0xff); \
    OUT_ARR[3] = (TYPE)((IN >> 24) & 0xff); \

#define VQDOT(IN1, IN2, TYPE1, TYPE2) \
    UNPACK_32_TO_8(IN1, TYPE1, unpacked_vs1) \
    UNPACK_32_TO_8(IN2, TYPE2, unpacked_vs2) \
    uint64_t result = unpacked_vs1[0]*unpacked_vs2[0] + \
                     unpacked_vs1[1]*unpacked_vs2[1] + \
                     unpacked_vs1[2]*unpacked_vs2[2] + \
                     unpacked_vs1[3]*unpacked_vs2[3] \

