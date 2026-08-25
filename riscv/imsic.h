// See LICENSE for license details.
#ifndef _RISCV_IMSIC_H
#define _RISCV_IMSIC_H
#include <memory>
#include <map>
#include <unordered_map>
#include "csrs.h"
#include "abstract_device.h"

#define IMSIC_FILE_LEVEL_NUM 2

// EIP/EIE0 to 63 but internally there are only 32 64b registers
#define IMSIC_NUM_EI_REGS 32
#define IMSIC_MMIO_PAGE_SIZE 0x1000

#define IMSIC_EIDELIVERY 0x70
#define IMSIC_EITHRESHOLD 0x72
#define IMSIC_EIP(x) (0x80 + x)
#define IMSIC_EIE(x) (0xc0 + x)

#define IMSIC_TOPI_IID   (0x7ffu << 16)
#define IMSIC_TOPI_IPRIO (0x7ffu << 0)

#define SETEIPNUM_LE 0x0
#define SETEIPNUM_BE 0x4

class imsic_file_t;
typedef std::shared_ptr<imsic_file_t> imsic_file_t_p;

class imsic_mmio_t : public abstract_device_t {
 public:
  imsic_mmio_t(imsic_file_t_p const imsic);
  bool load(reg_t addr, size_t len, uint8_t* bytes);
  bool store(reg_t addr, size_t len, const uint8_t* bytes);
  reg_t size() { return IMSIC_MMIO_PAGE_SIZE; }

 private:
  imsic_file_t_p const imsic;
};

class imsic_file_t {
 friend imsic_mmio_t;
 public:
  imsic_file_t(processor_t* const proc, reg_t mip_mask, size_t num_regs, bool v = false, reg_t vgein = 0);
  reg_t topei() const;
  void claimei (reg_t intr) const;
  void pendei (reg_t intr) const;
  void update_mip () const;
  bool delivery() const { return eidelivery->read(); }
  csr_t_p get_reg(reg_t reg) { return regmap.count(reg) ? regmap[reg] : nullptr; }
  csrmap_t regmap;

 private:
  processor_t* const proc;
  state_t* const state;
  reg_t mip_mask;
  bool v;
  reg_t vgein;
  csr_t_p eidelivery;
  csr_t_p eithreshold;
  std::vector<csr_t_p> eip;
  std::vector<csr_t_p> eie;
};

struct imsic_t {
  imsic_file_t_p m;
  imsic_file_t_p s;
  std::map<reg_t, imsic_file_t_p> vs;
  imsic_t(processor_t *proc, unsigned geilen);
  csrmap_t_p register_iprio(processor_t *proc, const reg_t type);
  bool vgein_valid(unsigned vgein) const { return vs.count(vgein); }
  csr_t_p get_vs_reg(unsigned vgein, reg_t reg) { return vs.count(vgein) ? vs[vgein]->get_reg(reg) : nullptr; }
  csrmap_t_p get_vs_regmap(reg_t vgein) {
    return vs.count(vgein) ? &vs[vgein]->regmap : nullptr;
  }
};
typedef std::shared_ptr<imsic_t> imsic_t_p;
#endif
