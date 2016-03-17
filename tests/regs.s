        .global main
main:
        j       main

write_regs:
        sd      x1, 0(a0)
        sd      x2, 8(a0)
        sd      x3, 16(a0)
        sd      x4, 24(a0)
        sd      x5, 32(a0)
        sd      x6, 40(a0)
        sd      x7, 48(a0)
        sd      x8, 56(a0)
        sd      x9, 64(a0)
        sd      x11, 72(a0)
        sd      x12, 80(a0)
        sd      x13, 88(a0)
        sd      x14, 96(a0)
        sd      x15, 104(a0)
        sd      x16, 112(a0)
        sd      x17, 120(a0)
        sd      x18, 128(a0)
        sd      x19, 136(a0)
        sd      x20, 144(a0)
        sd      x21, 152(a0)
        sd      x22, 160(a0)
        sd      x23, 168(a0)
        sd      x24, 176(a0)
        sd      x25, 184(a0)
        sd      x26, 192(a0)
        sd      x27, 200(a0)
        sd      x28, 208(a0)
        sd      x29, 216(a0)
        sd      x30, 224(a0)
        sd      x31, 232(a0)

        csrr    x1, 1   # fflags

all_done:
        j       all_done

data:
        .fill   64, 8, 0
