#ifndef _HTIF_CONTEXT_H
#define _HTIF_CONTEXT_H

// A replacement for ucontext.h, which is sadly deprecated.

#include <pthread.h>

#if defined(__GLIBC__)
# undef USE_UCONTEXT
# define USE_UCONTEXT
# include <ucontext.h>
# include <memory>
#include <limits.h>

#if (ULONG_MAX > UINT_MAX) // 64-bit systems only
#if (100*GLIB_MAJOR_VERSION+GLIB_MINOR_VERSION < 208)
#define GLIBC_64BIT_PTR_BUG
static_assert (sizeof(unsigned int)  == 4, "uint size doesn't match expected 32bit");
static_assert (sizeof(unsigned long) == 8, "ulong size doesn't match expected 64bit");
static_assert (sizeof(void*)         == 8, "ptr size doesn't match expected 64bit");
#endif
#endif /* ULONG_MAX > UINT_MAX */

#endif

class context_t
{
 public:
  context_t();
  ~context_t();
  void init(void (*func)(void*), void* arg);
  void switch_to();
  static context_t* current();
 private:
  context_t* creator;
  void (*func)(void*);
  void* arg;
#ifdef USE_UCONTEXT
  std::unique_ptr<ucontext_t> context;
#ifndef GLIBC_64BIT_PTR_BUG
  static void wrapper(context_t*);
#else
  static void wrapper(unsigned int, unsigned int);
#endif
#else
  pthread_t thread;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  volatile int flag;
  static void* wrapper(void*);
#endif
};

#endif
