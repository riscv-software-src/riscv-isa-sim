// See LICENSE for license details.
#include "imsic.h"
#include "processor.h"
#include "arith.h"
#include "sim.h"

class imsic_csr_t : public masked_csr_t {
 public:
  imsic_csr_t(processor_t* const proc, const reg_t addr, imsic_file_t *imsic_file, const reg_t mask = -1, const reg_t init = 0) : masked_csr_t(proc, addr, mask, init), imsic_file(*imsic_file) {
  }
 protected:
  virtual bool unlogged_write(const reg_t val) noexcept override {
    masked_csr_t::unlogged_write(val);
    // update MIP after write
    imsic_file.update_mip();
    // no logging of indirect registers
    return false;
  }
 private:
  imsic_file_t &imsic_file;
};

imsic_file_t::imsic_file_t(processor_t* const proc, reg_t mip_mask, size_t num_regs, bool v, reg_t vgein) : proc(proc), state(proc->get_state()), mip_mask(mip_mask), v(v), vgein(vgein) {
  auto xlen = proc->get_isa().get_max_xlen();
  csrmap[IMSIC_EIDELIVERY] = eidelivery = std::make_shared<imsic_csr_t>(proc, IMSIC_EIDELIVERY, this, 1, 0);
  csrmap[IMSIC_EITHRESHOLD] = eithreshold = std::make_shared<imsic_csr_t>(proc, IMSIC_EITHRESHOLD, this);
  for (size_t i = 0; i < IMSIC_NUM_EI_REGS; i++) {
    if (i < num_regs) {
      // 1st bits of eip/eie are hard-wired to 0
      const reg_t mask = i == 0 ? ~1ull : -1;
      eip.emplace_back(std::make_shared<imsic_csr_t>(proc, IMSIC_EIP(i * 2), this, mask));
      eie.emplace_back(std::make_shared<imsic_csr_t>(proc, IMSIC_EIE(i * 2), this, mask));
    } else {
      // make a const 0 register if index >= number of IMSIC files
      eip.emplace_back(std::make_shared<const_csr_t>(proc, IMSIC_EIP(i * 2), 0));
      eie.emplace_back(std::make_shared<const_csr_t>(proc, IMSIC_EIP(i * 2), 0));
    }
    if (xlen == 32) {
      csrmap[IMSIC_EIP(i * 2)] = std::make_shared<rv32_low_csr_t>(proc, IMSIC_EIP(i * 2), eip[i]);
      csrmap[IMSIC_EIP(i * 2 + 1)] = std::make_shared<rv32_high_csr_t>(proc, IMSIC_EIP(i * 2 + 1), eip[i]);
      csrmap[IMSIC_EIE(i * 2)] = std::make_shared<rv32_low_csr_t>(proc, IMSIC_EIE(i * 2), eie[i]);
      csrmap[IMSIC_EIE(i * 2 + 1)] = std::make_shared<rv32_high_csr_t>(proc, IMSIC_EIE(i * 2 + 1), eie[i]);
    } else {
      csrmap[IMSIC_EIP(i * 2)] = eip[i];
      csrmap[IMSIC_EIE(i * 2)] = eie[i];
    }
  }
}

reg_t imsic_file_t::topei() {
  reg_t thd = eithreshold->read();
  for (size_t i = 0; i < IMSIC_NUM_EI_REGS; i++) {
    reg_t ints = eip[i]->read() & eie[i]->read();
    if (ints == 0)
      continue;
    reg_t iid = ctz(ints) + i * 64;
    // When eithreshold is a nonzero value P, interrupt identities P and higher do not contribute to signaling interrupts
    if (thd && iid >= thd)
      return 0;
    return iid;
  }
  return 0;
}

void imsic_file_t::claimei(reg_t intr) {
  if (intr > 0 && intr < 64 * IMSIC_NUM_EI_REGS) {
    size_t reg = intr / 64;
    size_t idx = intr % 64;
    eip[reg]->write(eip[reg]->read() & ~(reg_t(1) << idx));
  }
}

void imsic_file_t::pendei(reg_t intr) {
  if (intr > 0 && intr < 64 * IMSIC_NUM_EI_REGS) {
    size_t reg = intr / 64;
    size_t idx = intr % 64;
    eip[reg]->write(eip[reg]->read() | (reg_t(1) << idx));
  }
}

void imsic_file_t::update_mip() {
  reg_t iid = eidelivery->read() ? topei() : 0;
  if (v) {
    // Privileged 9.2.4: Register hgeie selects the subset of guest external interrupts that cause a supervisor-level (HS-level) guest external interrupt.
    bool sgeip = proc->get_state()->hgeie->read() & proc->get_state()->hgeip->read();
    state->mip->backdoor_write_with_mask(MIP_SGEIP, sgeip ? MIP_SGEIP : 0);
  }
  // The enable bits in hgeie do not affect the VS-level external interrupt signal selected from hgeip by hstatus.VGEIN
  if (!v || (v && vgein == get_field(state->hstatus->read(), HSTATUS_VGEIN))) {
    // if not virtual, or virtual and vgein is selected in hstatus
    state->mip->backdoor_write_with_mask(mip_mask, iid ? mip_mask : 0);
  }
}

imsic_mmio_t::imsic_mmio_t(imsic_file_t_p const imsic) : imsic(imsic) {
}

bool imsic_mmio_t::load(reg_t addr UNUSED, size_t len, uint8_t* bytes) {
  if (len != 4)
    return false;
  // return 0 in all cases
  memset(bytes, 0, 4);
  return true;
}

bool imsic_mmio_t::store(reg_t addr, size_t len, const uint8_t* bytes) {
  if (len != 4)
    return false;
  if (addr == SETEIPNUM_LE || addr == SETEIPNUM_BE) {
    uint32_t intr;
    memcpy(&intr, bytes, sizeof(intr));
    if (addr == SETEIPNUM_BE)
      intr = swap(intr);
    imsic->pendei(intr);
  }
  return true;
}

imsic_t::imsic_t(processor_t *proc, unsigned geilen) {
  if (proc->extension_enabled_const(EXT_SMAIA))
    m = std::make_shared<imsic_file_t>(proc, MIP_MEIP, IMSIC_M_FILE_REGS);
  if (proc->extension_enabled_const(EXT_SSAIA)) {
    s = std::make_shared<imsic_file_t>(proc, MIP_SEIP, IMSIC_S_FILE_REGS);
    assert(geilen <= 63);
    for (size_t j = 1; j <= geilen; j++) {
      vs[j] = std::make_shared<imsic_file_t>(proc, MIP_VSEIP, IMSIC_VS_FILE_REGS, true, j);
    }
  }
}

std::string imsic_mmio_generate_dts(const sim_t* sim, const std::vector<std::string>& sargs UNUSED)
{
  auto cfg = sim->get_cfg();
  isa_parser_t isa(cfg.isa, cfg.priv);
  std::stringstream s;
  if (isa.extension_enabled(EXT_SMAIA)) {
    s << std::hex
      << "    IMSIC_M: imsics@" << IMSIC_M_BASE << " {\n"
         "      riscv,ipi-id = <0x01>;\n"
         "      riscv,num-ids = <0xff>;\n"
         "      reg = <0x00 0x" << IMSIC_M_BASE << " 0x00 0x" << sim->nprocs() * IMSIC_MMIO_PAGE_SIZE << ">;\n"
         "      interrupts-extended = <" << std::dec;
    for (size_t i = 0; i < sim->nprocs(); i++)
      s << "&CPU" << i << "_intc 0xb ";
    s << std::hex << ">;\n"
         "      msi-controller;\n"
         "      interrupt-controller;\n"
         "      #interrupt-cells = <0x00>;\n"
         "      compatible = \"riscv,imsics\";\n"
         "    };\n";
  }
  if (isa.extension_enabled(EXT_SSAIA)) {
    s << std::hex
      << "    IMSIC_S: imsics@" << IMSIC_S_BASE << " {\n"
         "      riscv,ipi-id = <0x01>;\n"
         "      riscv,num-ids = <0xff>;\n"
         "      riscv,guest-index-bits = <0x6>;\n"
         "      reg = <0x00 0x" << IMSIC_S_BASE << " 0x00 0x" << sim->nprocs() * IMSIC_MMIO_PAGE_SIZE * 64 << ">;\n"
         "      interrupts-extended = <" << std::dec;
    for (size_t i = 0; i < sim->nprocs(); i++)
      s << "&CPU" << i << "_intc 0x9 ";
    s << std::hex << ">;\n"
         "      msi-controller;\n"
         "      interrupt-controller;\n"
         "      #interrupt-cells = <0x00>;\n"
         "      compatible = \"riscv,imsics\";\n"
         "    };\n";
  }
  return s.str();
}

imsic_mmio_t* imsic_mmio_parse_from_fdt(const void* fdt UNUSED, const sim_t* sim UNUSED, reg_t* base UNUSED, const std::vector<std::string>& sargs UNUSED)
{
  return nullptr;
}

REGISTER_DEVICE(imsic_mmio, imsic_mmio_parse_from_fdt, imsic_mmio_generate_dts)
