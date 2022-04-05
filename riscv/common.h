// See LICENSE for license details.

#ifndef _RISCV_COMMON_H
#define _RISCV_COMMON_H

#ifdef __GNUC__
# define   likely(x) __builtin_expect(x, 1)
# define unlikely(x) __builtin_expect(x, 0)
# define NOINLINE __attribute__ ((noinline))
# define NORETURN __attribute__ ((noreturn))
#else
# define   likely(x) (x)
# define unlikely(x) (x)
# define NOINLINE
# define NORETURN
#endif

#endif
