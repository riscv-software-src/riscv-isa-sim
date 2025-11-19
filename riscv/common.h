// See LICENSE for license details.

#ifndef _RISCV_COMMON_H
#define _RISCV_COMMON_H

#ifdef __GNUC__
# define   likely(x) __builtin_expect(x, 1)
# define unlikely(x) __builtin_expect(x, 0)
# define NOINLINE __attribute__ ((noinline))
# define NORETURN __attribute__ ((noreturn))
# define ALWAYS_INLINE __attribute__ ((always_inline))
# define UNUSED __attribute__ ((unused))
#else
# define   likely(x) (x)
# define unlikely(x) (x)
# define NOINLINE
# define NORETURN
# define ALWAYS_INLINE
# define UNUSED
#endif

#ifndef __has_builtin
# define __has_builtin(x) 0
#endif

#if __has_cpp_attribute(assume)
# define assume(x) [[assume(x)]]
#elif __has_builtin(__builtin_assume)
# define assume(x) __builtin_assume(x)
#else
# define assume(x) ((void) 0)
#endif

#endif
