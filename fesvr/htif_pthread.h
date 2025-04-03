// See LICENSE for license details.

#ifndef _HTIF_PTHREAD_H
#define _HTIF_PTHREAD_H

#include "htif.h"
#include "context.h"
#include <deque>

class htif_pthread_t : public htif_t
{
 public:
  htif_pthread_t(int argc, char** argv);
  virtual ~htif_pthread_t();

  // target interface
  void send(const void* buf, size_t size);
  void recv(void* buf, size_t size);
  bool recv_nonblocking(void* buf, size_t size);

 protected:
  // host interface
  virtual ssize_t read(void* buf, size_t max_size);
  virtual ssize_t write(const void* buf, size_t size);

  virtual size_t chunk_align() { return 64; }
  virtual size_t chunk_max_size() { return 1024; }

 private:
  context_t host;
  context_t* target;
  std::deque<char> th_data;
  std::deque<char> ht_data;

  static void thread_main(void* htif);
};

#endif
