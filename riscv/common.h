#ifndef _RISCV_COMMON_H
#define _RISCV_COMMON_H

#define static_assert(x)	switch (x) case 0: case (x):

#define   likely(x) __builtin_expect(x, 1)
#define unlikely(x) __builtin_expect(x, 0)

#endif
