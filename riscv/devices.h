#ifndef _RISCV_DEVICES_H
#define _RISCV_DEVICES_H

#include "decode.h"
#include "mmio_plugin.h"
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <iostream>

class processor_t;

class abstract_device_t {
 public:
  virtual bool load(reg_t addr, size_t len, uint8_t* bytes) = 0;
  virtual bool store(reg_t addr, size_t len, const uint8_t* bytes) = 0;
  virtual ~abstract_device_t() {}
};

class bus_t : public abstract_device_t {
 public:
  bool load(reg_t addr, size_t len, uint8_t* bytes);
  bool store(reg_t addr, size_t len, const uint8_t* bytes);
  void add_device(reg_t addr, abstract_device_t* dev);

  std::pair<reg_t, abstract_device_t*> find_device(reg_t addr);

 private:
  std::map<reg_t, abstract_device_t*> devices;
};

class rom_device_t : public abstract_device_t {
 public:
  rom_device_t(std::vector<char> data);
  bool load(reg_t addr, size_t len, uint8_t* bytes);
  bool store(reg_t addr, size_t len, const uint8_t* bytes);
  const std::vector<char>& contents() { return data; }
 private:
  std::vector<char> data;
};

class mem_t : public abstract_device_t {
 public:
  mem_t(size_t size) : len(size) {
    if (!size)
      throw std::runtime_error("zero bytes of target memory requested");
    data = (char*)calloc(1, size);
    if (!data)
      throw std::runtime_error("couldn't allocate " + std::to_string(size) + " bytes of target memory");
  }
  mem_t(const mem_t& that) = delete;
  ~mem_t() { free(data); }

  bool load(reg_t addr, size_t len, uint8_t* bytes) { return false; }
  bool store(reg_t addr, size_t len, const uint8_t* bytes) { return false; }
  char* contents() { return data; }
  size_t size() { return len; }

 private:
  char* data;
  size_t len;
};

class clint_t : public abstract_device_t {
 public:
  clint_t(std::vector<processor_t*>&, uint64_t freq_hz, bool real_time);
  bool load(reg_t addr, size_t len, uint8_t* bytes);
  bool store(reg_t addr, size_t len, const uint8_t* bytes);
  size_t size() { return CLINT_SIZE; }
  void increment(reg_t inc);
  uint64_t get_mtime() {return mtime;}
 private:
  typedef uint64_t mtime_t;
  typedef uint64_t mtimecmp_t;
  typedef uint32_t msip_t;
  std::vector<processor_t*>& procs;
  uint64_t freq_hz;
  bool real_time;
  uint64_t real_time_ref_secs;
  uint64_t real_time_ref_usecs;
  mtime_t mtime;
  std::vector<mtimecmp_t> mtimecmp;
};

#ifdef ZJV_DEVICE_EXTENSTION 
class plic_t : public abstract_device_t {
 public:
  plic_t(std::vector<processor_t*>& procs, size_t num_source, size_t num_context);
  bool load(reg_t addr, size_t len, uint8_t* bytes);
  bool store(reg_t addr, size_t len, const uint8_t* bytes);
  size_t size() { return PLIC_SIZE; }

  uint32_t plic_claim (uint32_t contextid);
  void plic_update();
  bool plic_int_check(uint32_t contextid);
  void plic_irq (uint32_t irq);
 private:
  size_t num_source;
  size_t num_context;
  typedef uint32_t plic_reg_t;
  std::vector<processor_t*>& procs;
  std::vector<plic_reg_t> priority;
  std::vector<std::vector<plic_reg_t> > ie;
  std::vector<plic_reg_t> ip;
  std::vector<plic_reg_t> threshold;
  std::vector<std::vector<plic_reg_t> > claimed;

  typedef struct {
    uint32_t hartid;
    char mode;
  } context_t;
  std::vector<context_t> context;
};

class uart_t : public abstract_device_t {
 public:
  uart_t(plic_t* plic ,bool diffTest, std::string file_path);
  bool load(reg_t addr, size_t len, uint8_t* bytes);
  bool store(reg_t addr, size_t len, const uint8_t* bytes);
  size_t size() { return UART_SIZE; }
  void check_int();

 private:
  bool diffTest;
  plic_t* plic;
  std::ifstream file_fifo; 
  uint8_t uart_ier;
  uint8_t uart_isr;
  uint8_t uart_fcr;
  uint8_t uart_lcr;
  uint8_t uart_mcr;
  uint8_t uart_msr;
  uint8_t uart_spr;
  uint8_t uart_dll;
  uint8_t uart_dlm;
  uint8_t uart_psd;
};

#endif


class mmio_plugin_device_t : public abstract_device_t {
 public:
  mmio_plugin_device_t(const std::string& name, const std::string& args);
  virtual ~mmio_plugin_device_t() override;

  virtual bool load(reg_t addr, size_t len, uint8_t* bytes) override;
  virtual bool store(reg_t addr, size_t len, const uint8_t* bytes) override;

 private:
  mmio_plugin_t plugin;
  void* user_data;
};

#endif
