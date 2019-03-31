// See LICENSE for license details.

#include "htif_pthread.h"
#include <algorithm>
#include <stdio.h>

void htif_pthread_t::thread_main(void* arg)
{
  htif_pthread_t* htif = static_cast<htif_pthread_t*>(arg);
  htif->run();
  while (true)
    htif->target->switch_to();
}

htif_pthread_t::htif_pthread_t(int argc, char** argv)
    : htif_t(argc, argv)
{
  target = context_t::current();
  host.init(thread_main, this);
}

htif_pthread_t::~htif_pthread_t()
{
}

ssize_t htif_pthread_t::read(void* buf, size_t max_size)
{
  while (th_data.size() == 0)
    target->switch_to();

  size_t s = std::min(max_size, th_data.size());
  std::copy(th_data.begin(), th_data.begin() + s, (char*)buf);
  th_data.erase(th_data.begin(), th_data.begin() + s);

  return s;
}

ssize_t htif_pthread_t::write(const void* buf, size_t size)
{
  ht_data.insert(ht_data.end(), (const char*)buf, (const char*)buf + size);
  return size;
}

void htif_pthread_t::send(const void* buf, size_t size)
{
  th_data.insert(th_data.end(), (const char*)buf, (const char*)buf + size);
}

void htif_pthread_t::recv(void* buf, size_t size)
{
  while (!this->recv_nonblocking(buf, size))
    ;
}

bool htif_pthread_t::recv_nonblocking(void* buf, size_t size)
{
  if (ht_data.size() < size)
  {
    host.switch_to();
    return false;
  }

  std::copy(ht_data.begin(), ht_data.begin() + size, (char*)buf);
  ht_data.erase(ht_data.begin(), ht_data.begin() + size);
  return true;
}
