#ifndef _RISCV_ABSTRACT_DEVICE_H
#define _RISCV_ABSTRACT_DEVICE_H

#include "decode.h"
#include "common.h"
#include <cstdint>
#include <cstddef>
#include <string>

class sim_t;

class abstract_device_t {
 public:
  virtual bool load(reg_t addr, size_t len, uint8_t* bytes) = 0;
  virtual bool store(reg_t addr, size_t len, const uint8_t* bytes) = 0;
  virtual ~abstract_device_t() {}
  virtual void tick(reg_t UNUSED rtc_ticks) {}
};

// factory for devices which should show up in the DTS, and can be
// parameterized by parsing the DTS
class device_factory_t {
public:
  virtual abstract_device_t* parse_from_fdt(const void* fdt, const sim_t* sim, reg_t* base) const = 0;
  virtual std::string generate_dts(const sim_t* sim) const = 0;
  virtual ~device_factory_t() {}
};

#define REGISTER_DEVICE(name, parse, generate) \
  class name##_factory_t : public device_factory_t { \
  public: \
  name##_t* parse_from_fdt(const void* fdt, const sim_t* sim, reg_t* base) const override { return parse(fdt, sim, base); } \
  std::string generate_dts(const sim_t* sim) const override { return generate(sim); } \
  }; const device_factory_t *name##_factory = new name##_factory_t();

#endif
