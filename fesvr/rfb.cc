#include "rfb.h"
#include "memif.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sched.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <cstring>
#include <cinttypes>
using namespace std::placeholders;

rfb_t::rfb_t(int display)
  : sockfd(-1), afd(-1),
    memif(0), addr(0), width(0), height(0), bpp(0), display(display),
    thread(pthread_self()), fb1(0), fb2(0), read_pos(0),
    lock(PTHREAD_MUTEX_INITIALIZER)
{
  register_command(0, std::bind(&rfb_t::handle_configure, this, _1), "configure");
  register_command(1, std::bind(&rfb_t::handle_set_address, this, _1), "set_address");
}

void* rfb_thread_main(void* arg)
{
  ((rfb_t*)arg)->thread_main();
  return 0;
}

void rfb_t::thread_main()
{
  pthread_mutex_lock(&lock);

  int port = 5900 + display;
  sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    throw std::runtime_error("could not acquire tcp socket");

  struct sockaddr_in saddr, caddr;
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = INADDR_ANY;
  saddr.sin_port = htons(port);
  if (bind(sockfd, (struct sockaddr*)&saddr, sizeof(saddr)) < 0)
    throw std::runtime_error("could not bind to port " + std::to_string(port));
  if (listen(sockfd, 0) < 0)
    throw std::runtime_error("could not listen on port " + std::to_string(port));
 
  socklen_t clen = sizeof(caddr);
  afd = accept(sockfd, (struct sockaddr*)&caddr, &clen);
  if (afd < 0)
    throw std::runtime_error("could not accept connection");

  std::string version = "RFB 003.003\n";
  write(version);
  if (read() != version)
    throw std::runtime_error("bad client version");

  write(str(uint32_t(htonl(1))));

  read(); // clientinit

  std::string serverinit;
  serverinit += str(uint16_t(htons(width)));
  serverinit += str(uint16_t(htons(height)));
  serverinit += pixel_format();
  std::string name = "RISC-V";
  serverinit += str(uint32_t(htonl(name.length())));
  serverinit += name;
  write(serverinit);

  pthread_mutex_unlock(&lock);

  while (memif == NULL)
    sched_yield();

  while (memif != NULL)
  {
    std::string s = read();
    if (s.length() < 4)
      break; //throw std::runtime_error("bad command");

    switch (s[0])
    {
      case 0: set_pixel_format(s); break;
      case 2: set_encodings(s); break;
      case 3: break;
    }
  }

  pthread_mutex_lock(&lock);
  close(afd);
  close(sockfd);
  afd = -1;
  sockfd = -1;
  pthread_mutex_unlock(&lock);

  thread_main();
}

rfb_t::~rfb_t()
{
  memif = 0;
  if (!pthread_equal(pthread_self(), thread))
    pthread_join(thread, 0);
  delete [] fb1;
  delete [] fb2;
}

void rfb_t::set_encodings(const std::string& s)
{
  uint16_t n = htons(*(uint16_t*)&s[2]);
  for (size_t b = s.length(); b < 4U+4U*n; b += read().length());
}

void rfb_t::set_pixel_format(const std::string& s)
{
  if (s.length() != 20 || s.substr(4, 16) != pixel_format())
    throw std::runtime_error("bad pixel format");
}

void rfb_t::fb_update()
{
  std::string u;
  u += str(uint8_t(0));
  u += str(uint8_t(0));
  u += str(uint16_t(htons(1)));
  u += str(uint16_t(htons(0)));
  u += str(uint16_t(htons(0)));
  u += str(uint16_t(htons(width)));
  u += str(uint16_t(htons(height)));
  u += str(uint32_t(htonl(0)));
  u += std::string((char*)fb1, fb_bytes());

  try
  {
    write(u);
  }
  catch (std::runtime_error& e)
  {
  }
}

void rfb_t::tick()
{
  if (fb_bytes() == 0 || memif == NULL)
    return;

  memif->read(addr + read_pos, FB_ALIGN, const_cast<char*>(fb2 + read_pos));
  read_pos = (read_pos + FB_ALIGN) % fb_bytes();
  if (read_pos == 0)
  {
    std::swap(fb1, fb2);
    if (pthread_mutex_trylock(&lock) == 0)
    {
      fb_update();
      pthread_mutex_unlock(&lock);
    }
  }
}

std::string rfb_t::pixel_format()
{
  int red_bits = 8, green_bits = 8, blue_bits = 8;
  int bpp = red_bits + green_bits + blue_bits;
  while (bpp & (bpp-1)) bpp++;

  std::string fmt;
  fmt += str(uint8_t(bpp));
  fmt += str(uint8_t(red_bits + green_bits + blue_bits));
  fmt += str(uint8_t(0)); // little-endian
  fmt += str(uint8_t(1)); // true color
  fmt += str(uint16_t(htons((1<<red_bits)-1)));
  fmt += str(uint16_t(htons((1<<green_bits)-1)));
  fmt += str(uint16_t(htons((1<<blue_bits)-1)));
  fmt += str(uint8_t(blue_bits+green_bits));
  fmt += str(uint8_t(blue_bits));
  fmt += str(uint8_t(0));
  fmt += str(uint16_t(0)); // pad
  fmt += str(uint8_t(0)); // pad
  return fmt;
}

void rfb_t::write(const std::string& s)
{
  if ((size_t)::write(afd, s.c_str(), s.length()) != s.length())
    throw std::runtime_error("could not write");
}

std::string rfb_t::read()
{
  char buf[2048];
  ssize_t len = ::read(afd, buf, sizeof(buf));
  if (len < 0)
    throw std::runtime_error("could not read");
  if (len == sizeof(buf))
    throw std::runtime_error("received oversized packet");
  return std::string(buf, len);
}

void rfb_t::handle_configure(command_t cmd)
{
  if (fb1)
    throw std::runtime_error("you must only set the rfb configuration once");

  width = cmd.payload();
  height = cmd.payload() >> 16;

  bpp = cmd.payload() >> 32;
  if (bpp != 32)
    throw std::runtime_error("rfb requires 32 bpp true color");

  if (fb_bytes() % FB_ALIGN != 0)
    throw std::runtime_error("rfb size must be a multiple of " + std::to_string(FB_ALIGN));

  fb1 = new char[fb_bytes()];
  fb2 = new char[fb_bytes()];
  if (pthread_create(&thread, 0, rfb_thread_main, this))
    throw std::runtime_error("could not create thread");
  cmd.respond(1);
}

void rfb_t::handle_set_address(command_t cmd)
{
  addr = cmd.payload();
  if (addr % FB_ALIGN != 0)
    throw std::runtime_error("rfb address must be " + std::to_string(FB_ALIGN) + "-byte aligned");
  memif = &cmd.memif();
  cmd.respond(1);
}
