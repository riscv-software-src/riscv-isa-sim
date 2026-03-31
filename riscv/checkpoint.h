#ifndef RISCV_CHECKPOINT_H
#define RISCV_CHECKPOINT_H

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <memory>
#include <string>

#include "devices.h"
#include "platform.h"
#include "simif.h"

#define CHECKPOINT_BOOTROM_BASE (0x10000)
#define CHECKPOINT_BOOTROM_SIZE (0x10000)

struct checkpoint_config_t {
  const char *snapshot_load_name = nullptr;
  const char *snapshot_save_name = nullptr;
  bool snapshot_compress = false;
  bool snapshot_compress_zstd = false;
};

class checkpoint_t {
public:
  checkpoint_t(const checkpoint_config_t &config,
               simif_t *sim, bus_t *bus, clint_t *clint,
               std::string dtb, reg_t mainram_base,
               bool need_trampoline = true);
  ~checkpoint_t();

  void load();
  void load_cpu();
  void load_ram();
  void save();

private:
  checkpoint_config_t config;
  simif_t *sim;
  bus_t *bus;
  clint_t *clint;
  processor_t *proc;
  std::string dtb;
  reg_t mainram_base;
  bool need_trampoline;
  std::unique_ptr<rom_device_t> bootrom;
  std::shared_ptr<rom_device_t> trampoline_rom;

  void cpu_serialize(const char *save_name);
  void ram_serialize(const char *save_name);
  void host_serialize(const char *save_name);
  void cpu_deserialize(const char *load_name);
  void ram_deserialize(const char *load_name);
  void host_deserialize(const char *load_name);

  void compress_file(const std::string &source_file, const std::string &dest_file);
  void decompress_file(const std::string &source_file, const std::string &output_dir);

  void save_regs_file(const char *save_name);
  void create_boot_rom(const char *save_name);
};

#endif // RISCV_CHECKPOINT_H
