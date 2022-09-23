#include "device.h"
#include "term.h"
#include "memif.h"
#include <cassert>
#include <algorithm>
#include <climits>
#include <iostream>
#include <thread>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
using namespace std::placeholders;

device_t::device_t()
  : command_handlers(command_t::MAX_COMMANDS),
    command_names(command_t::MAX_COMMANDS)
{
  for (size_t cmd = 0; cmd < command_t::MAX_COMMANDS; cmd++)
    register_command(cmd, std::bind(&device_t::handle_null_command, this, _1), "");
  register_command(command_t::MAX_COMMANDS-1, std::bind(&device_t::handle_identify, this, _1), "identity");
}

void device_t::register_command(size_t cmd, command_func_t handler, const char* name)
{
  assert(cmd < command_t::MAX_COMMANDS);
  assert(strlen(name) < IDENTITY_SIZE);
  command_handlers[cmd] = handler;
  command_names[cmd] = name;
}

void device_t::handle_command(command_t cmd)
{
  command_handlers[cmd.cmd()](cmd);
}

void device_t::handle_null_command(command_t)
{
}

void device_t::handle_identify(command_t cmd)
{
  size_t what = cmd.payload() % command_t::MAX_COMMANDS;
  uint64_t addr = cmd.payload() / command_t::MAX_COMMANDS;

  char id[IDENTITY_SIZE] = {0};
  if (what == command_t::MAX_COMMANDS-1)
  {
    assert(strlen(identity()) < IDENTITY_SIZE);
    strcpy(id, identity());
  }
  else
    strcpy(id, command_names[what].c_str());

  cmd.memif().write(addr, IDENTITY_SIZE, id);
  cmd.respond(1);
}

bcd_t::bcd_t()
{
  register_command(0, std::bind(&bcd_t::handle_read, this, _1), "read");
  register_command(1, std::bind(&bcd_t::handle_write, this, _1), "write");
}

void bcd_t::handle_read(command_t cmd)
{
  pending_reads.push(cmd);
}

void bcd_t::handle_write(command_t cmd)
{
  canonical_terminal_t::write(cmd.payload());
}

void bcd_t::tick()
{
  int ch;
  if (!pending_reads.empty() && (ch = canonical_terminal_t::read()) != -1)
  {
    pending_reads.front().respond(0x100 | ch);
    pending_reads.pop();
  }
}

disk_t::disk_t(const char* fn)
{
  fd = ::open(fn, O_RDWR);
  if (fd < 0)
    throw std::runtime_error("could not open " + std::string(fn));

  register_command(0, std::bind(&disk_t::handle_read, this, _1), "read");
  register_command(1, std::bind(&disk_t::handle_write, this, _1), "write");

  struct stat st;
  if (fstat(fd, &st) < 0)
    throw std::runtime_error("could not stat " + std::string(fn));

  size = st.st_size;
  id = "disk size=" + std::to_string(size);
}

disk_t::~disk_t()
{
  close(fd);
}

void disk_t::handle_read(command_t cmd)
{
  request_t req;
  cmd.memif().read(cmd.payload(), sizeof(req), &req);

  std::vector<uint8_t> buf(req.size);
  if ((size_t)::pread(fd, buf.data(), buf.size(), req.offset) != req.size)
    throw std::runtime_error("could not read " + id + " @ " + std::to_string(req.offset));

  cmd.memif().write(req.addr, buf.size(), buf.data());
  cmd.respond(req.tag);
}

void disk_t::handle_write(command_t cmd)
{
  request_t req;
  cmd.memif().read(cmd.payload(), sizeof(req), &req);

  std::vector<uint8_t> buf(req.size);
  cmd.memif().read(req.addr, buf.size(), buf.data());

  if ((size_t)::pwrite(fd, buf.data(), buf.size(), req.offset) != req.size)
    throw std::runtime_error("could not write " + id + " @ " + std::to_string(req.offset));

  cmd.respond(req.tag);
}

device_list_t::device_list_t()
  : devices(command_t::MAX_COMMANDS, &null_device), num_devices(0)
{
}

void device_list_t::register_device(device_t* dev)
{
  num_devices++;
  assert(num_devices < command_t::MAX_DEVICES);
  devices[num_devices-1] = dev;
}

void device_list_t::handle_command(command_t cmd)
{
  devices[cmd.device()]->handle_command(cmd);
}

void device_list_t::tick()
{
  for (size_t i = 0; i < num_devices; i++)
    devices[i]->tick();
}
