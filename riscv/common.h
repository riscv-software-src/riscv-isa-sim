// See LICENSE for license details.

#ifndef _RISCV_COMMON_H
#define _RISCV_COMMON_H

#define   likely(x) __builtin_expect(x, 1)
#define unlikely(x) __builtin_expect(x, 0)

#endif
