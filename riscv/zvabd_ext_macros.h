#ifndef RISCV_ZVABD_MACROS_H_
#define RISCV_ZVABD_MACROS_H_

#define require_zvabd \
  do { \
    require_vector(true); \
    require_extension(EXT_ZVABD); \
  } while (0)

#define DO_ABD(N, M)  ((N) > (M) ? (N) - (M) : (M) - (N))

#endif
