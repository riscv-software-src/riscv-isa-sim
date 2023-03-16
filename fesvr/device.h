#ifndef _DEVICE_H
#define _DEVICE_H

#include <vector>
#include <queue>
#include <cstring>
#include <string>
#include <functional>
#include <cstdint>

class memif_t;

class command_t
{
 public:
  typedef std::function<void(uint64_t)> callback_t;
  command_t(memif_t& memif, uint64_t tohost, callback_t cb)
    : _memif(memif), tohost(tohost), cb(cb) {}

  memif_t& memif() { return _memif; }
  uint8_t device() { return tohost >> 56; }
  uint8_t cmd() { return tohost >> 48; }
  uint64_t payload() { return tohost << 16 >> 16; }
  void respond(uint64_t resp) { cb((tohost >> 48 << 48) | (resp << 16 >> 16)); }

  static const size_t MAX_COMMANDS = 256;
  static const size_t MAX_DEVICES = 256;

 private:
  memif_t& _memif;
  uint64_t tohost;
  callback_t cb;
};

class device_t
{
 public:
  device_t();
  virtual ~device_t() {}
  virtual const char* identity() = 0;
  virtual void tick() {}

  void handle_command(command_t cmd);

 protected:
  typedef std::function<void(command_t)> command_func_t;
  void register_command(size_t, command_func_t, const char*);

 private:
  device_t& operator = (const device_t&); // disallow
  device_t(const device_t&); // disallow

  static const size_t IDENTITY_SIZE = 64;
  void handle_null_command(command_t cmd);
  void handle_identify(command_t cmd);

  std::vector<command_func_t> command_handlers;
  std::vector<std::string> command_names;
};

class bcd_t : public device_t
{
 public:
  bcd_t();
  const char* identity() { return "bcd"; }
  void tick();

 private:
  void handle_read(command_t cmd);
  void handle_write(command_t cmd);

  std::queue<command_t> pending_reads;
};

class disk_t : public device_t
{
 public:
  disk_t(const char* fn);
  ~disk_t();
  const char* identity() { return id.c_str(); }

 private:
  struct request_t
  {
    uint64_t addr;
    uint64_t offset;
    uint64_t size;
    uint64_t tag;
  };

  void handle_read(command_t cmd);
  void handle_write(command_t cmd);

  std::string id;
  size_t size;
  int fd;
};

class null_device_t : public device_t
{
 public:
  const char* identity() { return ""; }
};

class device_list_t
{
 public:
  device_list_t();
  void register_device(device_t* dev);
  void handle_command(command_t cmd);
  void tick();

 private:
  std::vector<device_t*> devices;
  null_device_t null_device;
  size_t num_devices;
};

#endif
