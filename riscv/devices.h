#ifndef _RISCV_DEVICES_H
#define _RISCV_DEVICES_H

#include "decode.h"
#include "abstract_device.h"
#include "abstract_interrupt_controller.h"
#include "platform.h"
#include <map>
#include <queue>
#include <vector>
#include <utility>
#include <cassert>

class processor_t;
class simif_t;

class bus_t : public abstract_device_t {
 public:
  bus_t();

  // the fallback device owns all addresses not owned by other devices
  bus_t(abstract_device_t* fallback);

  bool load(reg_t addr, size_t len, uint8_t* bytes) override;
  bool store(reg_t addr, size_t len, const uint8_t* bytes) override;
  reg_t size() override;
  void add_device(reg_t addr, abstract_device_t* dev);

  std::pair<reg_t, abstract_device_t*> find_device(reg_t addr, size_t len);

 private:
  std::map<reg_t, abstract_device_t*> devices;
  abstract_device_t* fallback;
};

class rom_device_t : public abstract_device_t {
 public:
  rom_device_t(std::vector<char> data);
  bool load(reg_t addr, size_t len, uint8_t* bytes) override;
  bool store(reg_t addr, size_t len, const uint8_t* bytes) override;
  reg_t size() override { return data.size(); }
  const std::vector<char>& contents() { return data; }
 private:
  std::vector<char> data;
};

class abstract_mem_t : public abstract_device_t {
 public:
  virtual ~abstract_mem_t() = default;

  virtual char* contents(reg_t addr) = 0;
  virtual void dump(std::ostream& o) = 0;
};

class mem_t : public abstract_mem_t {
 public:
  mem_t(reg_t size);
  mem_t(const mem_t& that) = delete;
  ~mem_t() override;

  bool load(reg_t addr, size_t len, uint8_t* bytes) override { return load_store(addr, len, bytes, false); }
  bool store(reg_t addr, size_t len, const uint8_t* bytes) override { return load_store(addr, len, const_cast<uint8_t*>(bytes), true); }
  char* contents(reg_t addr) override;
  reg_t size() override { return sz; }
  void dump(std::ostream& o) override;

 private:
  bool load_store(reg_t addr, size_t len, uint8_t* bytes, bool store);

  std::map<reg_t, char*> sparse_memory_map;
  reg_t sz;
};

class abstract_sim_if_t {
public:
  virtual ~abstract_sim_if_t() = default;
  virtual bool load(reg_t addr, size_t len, uint8_t* bytes) = 0;
  virtual bool store(reg_t addr, size_t len, const uint8_t* bytes) = 0;
};

class external_sim_device_t : public abstract_device_t {
public:
  external_sim_device_t(abstract_sim_if_t* sim);
  void set_simulator(abstract_sim_if_t* sim);
  bool load(reg_t addr, size_t len, uint8_t* bytes) override;
  bool store(reg_t addr, size_t len, const uint8_t* bytes) override;
  reg_t size() override;

private:
  abstract_sim_if_t* external_simulator;
};

class clint_t : public abstract_device_t {
 public:
  clint_t(const simif_t*, uint64_t freq_hz, bool real_time);
  bool load(reg_t addr, size_t len, uint8_t* bytes) override;
  bool store(reg_t addr, size_t len, const uint8_t* bytes) override;
  reg_t size() override { return CLINT_SIZE; }
  void tick(reg_t rtc_ticks) override;
  uint64_t get_mtimecmp(reg_t hartid) { return mtimecmp[hartid]; }
  uint64_t get_mtime() { return mtime; }
 private:
  typedef uint64_t mtime_t;
  typedef uint64_t mtimecmp_t;
  typedef uint32_t msip_t;
  const simif_t* sim;
  uint64_t freq_hz;
  bool real_time;
  uint64_t real_time_ref_secs;
  uint64_t real_time_ref_usecs;
  mtime_t mtime;
  std::map<size_t, mtimecmp_t> mtimecmp;
};

#define PLIC_MAX_DEVICES 1024

struct plic_context_t {
  plic_context_t(processor_t* proc, bool mmode)
    : proc(proc), mmode(mmode)
  {}

  processor_t *proc;
  bool mmode;

  uint8_t priority_threshold {};
  uint32_t enable[PLIC_MAX_DEVICES/32] {};
  uint32_t pending[PLIC_MAX_DEVICES/32] {};
  uint8_t pending_priority[PLIC_MAX_DEVICES] {};
  uint32_t claimed[PLIC_MAX_DEVICES/32] {};
};

class plic_t : public abstract_device_t, public abstract_interrupt_controller_t {
 public:
  plic_t(const simif_t*, uint32_t ndev);
  bool load(reg_t addr, size_t len, uint8_t* bytes) override;
  bool store(reg_t addr, size_t len, const uint8_t* bytes) override;
  void set_interrupt_level(uint32_t id, int lvl) override;
  reg_t size() override { return PLIC_SIZE; }
 private:
  std::vector<plic_context_t> contexts;
  uint32_t num_ids;
  uint32_t num_ids_word;
  uint32_t max_prio;
  uint8_t priority[PLIC_MAX_DEVICES];
  uint32_t level[PLIC_MAX_DEVICES/32];
  uint32_t context_best_pending(const plic_context_t *c);
  void context_update(const plic_context_t *context);
  uint32_t context_claim(plic_context_t *c);
  bool priority_read(reg_t offset, uint32_t *val);
  bool priority_write(reg_t offset, uint32_t val);
  bool pending_read(reg_t offset, uint32_t *val);
  bool context_enable_read(const plic_context_t *context,
                           reg_t offset, uint32_t *val);
  bool context_enable_write(plic_context_t *context,
                            reg_t offset, uint32_t val);
  bool context_read(plic_context_t *context,
                    reg_t offset, uint32_t *val);
  bool context_write(plic_context_t *context,
                     reg_t offset, uint32_t val);
};

#define APLIC_MAX_DEVICES 1024
class aplic_t : public abstract_device_t, public abstract_interrupt_controller_t {
 public:
  aplic_t(const simif_t*, aplic_t *parent);
  bool load(reg_t addr, size_t len, uint8_t* bytes);
  bool store(reg_t addr, size_t len, const uint8_t* bytes);
  void set_interrupt_level(uint32_t id, int lvl);
  reg_t size() { return APLIC_SIZE; }
  void set_child(aplic_t *c) { child = c; }
  void delegate(uint32_t id, bool dm);
  bool delegated(uint32_t id);
  uint32_t get_deleg_mask(uint32_t idx);
 private:
  const simif_t *simif;
  class aplic_t *parent;
  class aplic_t *child;
  uint32_t domaincfg;
  uint32_t sourcecfg[APLIC_MAX_DEVICES];
  uint32_t mmsiaddrcfgh;
  uint32_t ie[APLIC_MAX_DEVICES / 32];
  uint32_t target[APLIC_MAX_DEVICES];
  uint32_t level[APLIC_MAX_DEVICES / 32];
  uint32_t genmsi;
  // deleg_mask can be directly applied to other packed registers
  uint32_t deleg_mask[APLIC_MAX_DEVICES / 32];

  bool interrupt_enabled(uint32_t id);
  bool accessible(uint32_t id);
  void send_msi(uint32_t proc_id, uint32_t guest, uint32_t eiid);
  void send_msi(uint32_t id);
  void update_interrupt(uint32_t id);
  void update_interrupt_masked(uint32_t idx, uint32_t mask);
};
class aplic_m_t : public aplic_t {
 public:
  aplic_m_t(const simif_t* simif, aplic_t *parent) : aplic_t(simif, parent) {}
};
class aplic_s_t : public aplic_t {
 public:
  aplic_s_t(const simif_t* simif, aplic_t *parent) : aplic_t(simif, parent) {}
};

class ns16550_t : public abstract_device_t {
 public:
  ns16550_t(abstract_interrupt_controller_t *intctrl,
            uint32_t interrupt_id, uint32_t reg_shift, uint32_t reg_io_width);
  bool load(reg_t addr, size_t len, uint8_t* bytes) override;
  bool store(reg_t addr, size_t len, const uint8_t* bytes) override;
  void tick(reg_t rtc_ticks) override;
  reg_t size() override { return NS16550_SIZE; }
 private:
  abstract_interrupt_controller_t *intctrl;
  uint32_t interrupt_id;
  uint32_t reg_shift;
  uint32_t reg_io_width;
  std::queue<uint8_t> rx_queue;
  uint8_t dll;
  uint8_t dlm;
  uint8_t iir;
  uint8_t ier;
  uint8_t fcr;
  uint8_t lcr;
  uint8_t mcr;
  uint8_t lsr;
  uint8_t msr;
  uint8_t scr;
  void update_interrupt(void);
  uint8_t rx_byte(void);
  void tx_byte(uint8_t val);

  int backoff_counter;
  static const int MAX_BACKOFF = 16;
};

template<typename T>
void write_little_endian_reg(T* word, reg_t addr, size_t len, const uint8_t* bytes)
{
  assert(len <= sizeof(T));

  for (size_t i = 0; i < len; i++) {
    const int shift = 8 * ((addr + i) % sizeof(T));
    *word = (*word & ~(T(0xFF) << shift)) | (T(bytes[i]) << shift);
  }
}

template<typename T>
void read_little_endian_reg(T word, reg_t addr, size_t len, uint8_t* bytes)
{
  assert(len <= sizeof(T));

  for (size_t i = 0; i < len; i++) {
    const int shift = 8 * ((addr + i) % sizeof(T));
    bytes[i] = word >> shift;
  }
}

#endif
