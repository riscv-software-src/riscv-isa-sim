#include "context.h"
#include <assert.h>
#include <sched.h>
#include <stdlib.h>

static __thread context_t* cur;

context_t::context_t()
  : creator(NULL), func(NULL), arg(NULL),
#ifndef USE_UCONTEXT
    mutex(PTHREAD_MUTEX_INITIALIZER),
    cond(PTHREAD_COND_INITIALIZER), flag(0)
#else
    context(new ucontext_t)
#endif
{
}

#ifdef USE_UCONTEXT
#ifndef GLIBC_64BIT_PTR_BUG
void context_t::wrapper(context_t* ctx)
{
#else
void context_t::wrapper(unsigned int hi, unsigned int lo)
{
  context_t* ctx = reinterpret_cast<context_t*>(static_cast<unsigned long>(lo) | (static_cast<unsigned long>(hi) << 32));
#endif
  ctx->creator->switch_to();
  ctx->func(ctx->arg);
}
#else
void* context_t::wrapper(void* a)
{
  context_t* ctx = static_cast<context_t*>(a);
  cur = ctx;
  ctx->creator->switch_to();

  ctx->func(ctx->arg);
  return NULL;
}
#endif

void context_t::init(void (*f)(void*), void* a)
{
  func = f;
  arg = a;
  creator = current();

#ifdef USE_UCONTEXT
  getcontext(context.get());
  context->uc_link = creator->context.get();
  context->uc_stack.ss_size = 64*1024;
  context->uc_stack.ss_sp = new void*[context->uc_stack.ss_size/sizeof(void*)];
#ifndef GLIBC_64BIT_PTR_BUG
  makecontext(context.get(), (void(*)(void))&context_t::wrapper, 1, this);
#else
  unsigned int hi(reinterpret_cast<unsigned long>(this) >> 32);
  unsigned int lo(reinterpret_cast<unsigned long>(this));
  makecontext(context.get(), (void(*)(void))&context_t::wrapper, 2, hi, lo);
#endif
  switch_to();
#else
  assert(flag == 0);

  pthread_mutex_lock(&creator->mutex);
  creator->flag = 0;
  if (pthread_create(&thread, NULL, &context_t::wrapper, this) != 0)
    abort();
  pthread_detach(thread);
  while (!creator->flag)
    pthread_cond_wait(&creator->cond, &creator->mutex);
  pthread_mutex_unlock(&creator->mutex);
#endif
}

context_t::~context_t()
{
  assert(this != cur);
}

void context_t::switch_to()
{
  assert(this != cur);
#ifdef USE_UCONTEXT
  context_t* prev = cur;
  cur = this;
  if (swapcontext(prev->context.get(), context.get()) != 0)
    abort();
#else
  cur->flag = 0;
  this->flag = 1;
  pthread_mutex_lock(&this->mutex);
  pthread_cond_signal(&this->cond);
  pthread_mutex_unlock(&this->mutex);
  pthread_mutex_lock(&cur->mutex);
  while (!cur->flag)
    pthread_cond_wait(&cur->cond, &cur->mutex);
  pthread_mutex_unlock(&cur->mutex);
#endif
}

context_t* context_t::current()
{
  if (cur == NULL)
  {
    cur = new context_t;
#ifdef USE_UCONTEXT
    getcontext(cur->context.get());
#else
    cur->thread = pthread_self();
    cur->flag = 1;
#endif
  }
  return cur;
}
