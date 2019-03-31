#ifndef _RFB_H
#define _RFB_H

#include "device.h"
#include "memif.h"
#include <pthread.h>

// remote frame buffer
class rfb_t : public device_t
{
 public:
  rfb_t(int display = 0);
  ~rfb_t();
  void tick();
  std::string name() { return "RISC-V"; }
  const char* identity() { return "rfb"; }

 private:
  template <typename T>
  std::string str(T x)
  {
    return std::string((char*)&x, sizeof(x));
  }
  size_t fb_bytes() { return size_t(width) * height * bpp/8; }
  void thread_main();
  friend void* rfb_thread_main(void*);
  std::string pixel_format();
  void fb_update(const std::string& s);
  void set_encodings(const std::string& s);
  void set_pixel_format(const std::string& s);
  void write(const std::string& s);
  std::string read();
  void handle_configure(command_t cmd);
  void handle_set_address(command_t cmd);

  int sockfd;
  int afd;
  memif_t* memif;
  reg_t addr;
  uint16_t width;
  uint16_t height;
  uint16_t bpp;
  int display;
  pthread_t thread;
  volatile char* volatile fb1;
  volatile char* volatile fb2;
  size_t read_pos;
  pthread_mutex_t lock;

  static const int FB_ALIGN = 256;
};

#endif
