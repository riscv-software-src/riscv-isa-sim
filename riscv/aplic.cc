// See LICENSE for license details.
#include "devices.h"
#include "processor.h"
#include "arith.h"
#include "sim.h"
#include "dts.h"

#define DOMAINCFG       0x0
#define SOURCECFG_BASE  0x4
#define MMSIADDRCFG     0x1bc0
#define MMSIADDRCFGH    0x1bc4
#define SMSIADDRCFG     0x1bc8
#define SMSIADDRCFGH    0x1bcc
#define SETIP_BASE      0x1c00
#define SETIPNUM        0x1cdc
#define IN_CLRIP_BASE   0x1d00
#define CLRIPNUM        0x1ddc
#define SETIE_BASE      0x1e00
#define SETIENUM        0x1edc
#define CLRIE_BASE      0x1f00
#define CLRIENUM        0x1fdc
#define GENMSI          0x3000
#define TARGET_BASE     0x3004
#define IDC             0x4000

#define DOMAINCFG_IE_MASK       0x100
#define DOMAINCFG_DM_MASK       0x4

#define SOURCECFG_D_MASK        0x400
#define SOURCECFG_CHILD_MASK    0x3ff
#define SOURCECFG_SM_MASK       0x7

#define MMSIADDRCFGH_L_MASK     0x80000000

#define GENMSI_HART_MASK        0xfffc0000
#define GENMSI_BUSY_MASK        0x1000
#define GENMSI_EIID_MASK        0x7ff

#define TARGET_HART_MASK        0xfffc0000
#define TARGET_GUEST_MASK       0x3f000
#define TARGET_EIID_MASK        0x7ff

aplic_t::aplic_t(const simif_t *simif, aplic_t *parent) : simif(simif), parent(parent), child(nullptr), domaincfg((0x80 << 24) | DOMAINCFG_DM_MASK), sourcecfg(), mmsiaddrcfgh(MMSIADDRCFGH_L_MASK), ie(), target(), level(), genmsi(0), deleg_mask()
{
}

void aplic_t::delegate(uint32_t id, bool dm)
{
  if (!id || id >= APLIC_MAX_DEVICES)
    return;

  if (!child) {
    // child's sourcecfg = 0 when interrupt is changed to delegated
    if (!parent->delegated(id) && dm)
      sourcecfg[id] = 0;
  } else {
    uint32_t mask = (1 << (id % 32));
    deleg_mask[id / 32] &= ~mask;
    deleg_mask[id / 32] |= dm ? mask : 0;
    child->delegate(id, dm);
  }
}

bool aplic_t::delegated(uint32_t id)
{
  if (!id || id >= APLIC_MAX_DEVICES)
    return false;
  return (sourcecfg[id] & SOURCECFG_D_MASK);
}

uint32_t aplic_t::get_deleg_mask(uint32_t idx) {
    if (idx >= APLIC_MAX_DEVICES / 32)
      return 0;
    if (parent)
      return parent->get_deleg_mask(idx);
    return deleg_mask[idx];
  }

bool aplic_t::interrupt_enabled(uint32_t id)
{
  if (!id || id >= APLIC_MAX_DEVICES)
    return false;
  if (parent && !parent->delegated(id))
    return false;
  // Domain interrupt enabled, source IE and source mode != inactive (0)
  return (domaincfg & DOMAINCFG_IE_MASK) && (ie[id / 32] & (1 << (id % 32))) && (sourcecfg[id] & SOURCECFG_SM_MASK);
}

bool aplic_t::accessible(uint32_t id)
{
  if (!id || id >= APLIC_MAX_DEVICES)
    return false;
  if (!parent)
    return true;
  return parent->delegated(id);
}

void aplic_t::set_interrupt_level(uint32_t id, int lvl)
{
  if (child && delegated(id)) {
    child->set_interrupt_level(id, lvl);
    return;
  }

  if (!accessible(id))
    return;

  uint32_t mask = 1 << (id % 32);
  level[id / 32] &= ~mask;
  level[id / 32] |= lvl ? mask : 0;

  update_interrupt(id);
}

void aplic_t::send_msi(uint32_t proc_id, uint32_t guest, uint32_t eiid)
{
  auto &procs = simif->get_harts();
  auto it = procs.find(proc_id);
  if (!eiid || it == procs.end())
    return;

  processor_t *proc = it->second;
  if (!parent) {
    proc->imsic->m->pendei(eiid);
  } else if (guest) {
    if (proc->imsic->vs.count(guest))
      proc->imsic->vs[guest]->pendei(eiid);
  } else {
    proc->imsic->s->pendei(eiid);
  }
}

void aplic_t::send_msi(uint32_t id)
{
  if (id >= APLIC_MAX_DEVICES)
    return;
  auto tgt = target[id];
  send_msi(get_field(tgt, TARGET_HART_MASK), get_field(tgt, TARGET_GUEST_MASK), get_field(tgt, TARGET_EIID_MASK));
}

void aplic_t::update_interrupt(uint32_t id)
{
  // check level and if interrupt enabled, send MSI if necessary
  if (id >= APLIC_MAX_DEVICES)
    return;
  bool lvl = level[id / 32] & (1 << (id % 32));
  if (lvl && interrupt_enabled(id))
    send_msi(id);
}

void aplic_t::update_interrupt_masked(uint32_t idx, uint32_t mask)
{
  if (idx >= APLIC_MAX_DEVICES / 32)
    return;
  while(mask) {
    auto id = ctz(mask);
    update_interrupt(idx * 32 + id);
    mask &= ~(1 << id);
  }
}

bool aplic_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  uint32_t val = 0;

  if (len != 4) {
    return false;
  }

  addr &= ~reg_t(3);

  if (addr == DOMAINCFG) {
    val = domaincfg;
  } else if (addr >= SOURCECFG_BASE && addr < MMSIADDRCFG) {
    auto idx = (addr - SOURCECFG_BASE) / 4 + 1;
    if (accessible(idx))
      val = sourcecfg[idx];
  } else if (addr == MMSIADDRCFGH) {
    val = mmsiaddrcfgh;
  } else if (addr >= IN_CLRIP_BASE && addr < CLRIPNUM) {
    auto idx = (addr - IN_CLRIP_BASE) / 4;
    val = level[idx];
  } else if (addr >= SETIE_BASE && addr < SETIENUM) {
    auto idx = (addr - SETIE_BASE) / 4;
    val = ie[idx];
  } else if (addr == GENMSI) {
    val = genmsi;
  } else if (addr >= TARGET_BASE && addr < IDC) {
    auto idx = (addr - TARGET_BASE) / 4 + 1;
    if (accessible(idx))
      val = target[idx];
  }

  memcpy(bytes, (uint8_t *)&val, len);

  return true;
}

bool aplic_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  uint32_t val = 0;

  if (len != 4) {
    return false;
  }

  addr &= ~reg_t(3);
  memcpy((uint8_t *)&val, bytes, len);

  if (addr == DOMAINCFG) {
    domaincfg &= ~DOMAINCFG_IE_MASK;
    domaincfg |= val & DOMAINCFG_IE_MASK;
  } else if (addr >= SOURCECFG_BASE && addr < MMSIADDRCFG) {
    auto idx = (addr - SOURCECFG_BASE) / 4 + 1;
    // write to D bit sets child's sourcecfg to 0
    // SM = inactive (0) or Level1 (6)
    if (accessible(idx)) {
      if (!child && (val & SOURCECFG_D_MASK))
        val = 0;
      else if (child)
        delegate(idx, val & SOURCECFG_D_MASK);

      // force SM to be 6 if non-zero
      if (val & SOURCECFG_SM_MASK)
        val = set_field(val, SOURCECFG_SM_MASK, 6);

      sourcecfg[idx] = val;
    }
  } else if (addr >= SETIP_BASE && addr < SETIPNUM) {
    auto idx = (addr - SETIP_BASE) / 4;
    val &= get_deleg_mask(idx);
    update_interrupt_masked(idx, val);
  } else if (addr == SETIPNUM) {
    update_interrupt(val);
  } else if (addr >= SETIE_BASE && addr < SETIENUM) {
    auto idx = (addr - SETIE_BASE) / 4;
    val &= get_deleg_mask(idx);
    ie[idx] |= val;
  } else if (addr == SETIENUM) {
    if (accessible(val))
      ie[val / 32] |= 1 << (val % 32);
  } else if (addr >= CLRIE_BASE && addr < CLRIENUM) {
    auto idx = (addr - CLRIE_BASE) / 4;
    val &= get_deleg_mask(idx);
    ie[idx] &= ~val;
  } else if (addr == CLRIENUM) {
    if (accessible(val))
      ie[val / 32] &= ~(1 << (val % 32));
  } else if (addr == GENMSI) {
    genmsi = val & (GENMSI_HART_MASK | GENMSI_EIID_MASK);
    send_msi(get_field(genmsi, GENMSI_HART_MASK), 0, get_field(genmsi, GENMSI_EIID_MASK));
  } else if (addr >= TARGET_BASE && addr < IDC) {
    auto idx = (addr - TARGET_BASE) / 4 + 1;
    if (accessible(val))
      target[idx] = val;
  }

  return true;
}

std::string aplic_m_generate_dts(const sim_t* sim, const std::vector<std::string>& sargs UNUSED)
{
  auto cfg = sim->get_cfg();
  isa_parser_t isa(cfg.isa, cfg.priv);
  std::stringstream s;
  if (isa.extension_enabled(EXT_SMAIA)) {
    s << std::hex
      << "    APLIC_M: aplic@" << APLIC_M_BASE << " {\n"
         "      riscv,delegate = <&APLIC_S 0x01 0x35>;\n"
         "      riscv,children = <&APLIC_S>;\n"
         "      riscv,num-sources = <0x35>;\n"
         "      reg = <0x00 0x" << APLIC_M_BASE << " 0x00 0x" << APLIC_SIZE << ">;\n"
         "      msi-parent = <&IMSIC_M>;\n"
         "      interrupt-controller;\n"
         "      #interrupt-cells = <0x02>;\n"
         "      compatible = \"riscv,aplic\";\n"
         "    };\n";
  }
  return s.str();
}

std::string aplic_s_generate_dts(const sim_t* sim, const std::vector<std::string>& sargs UNUSED)
{
  auto cfg = sim->get_cfg();
  isa_parser_t isa(cfg.isa, cfg.priv);
  std::stringstream s;
  if (isa.extension_enabled(EXT_SSAIA)) {
    s << std::hex
      << "    APLIC_S: aplic@" << APLIC_S_BASE << " {\n"
         "      riscv,num-sources = <0x35>;\n"
         "      reg = <0x00 0x" << APLIC_S_BASE << " 0x00 0x" << APLIC_SIZE << ">;\n"
         "      msi-parent = <&IMSIC_S>;\n"
         "      interrupt-controller;\n"
         "      #interrupt-cells = <0x02>;\n"
         "      compatible = \"riscv,aplic\";\n"
         "    };\n";
  }
  return s.str();
}

aplic_m_t* aplic_m_parse_from_fdt(const void* fdt, const sim_t* sim, reg_t* base, const std::vector<std::string>& sargs UNUSED)
{
  reg_t aplic_m_base, aplic_s_base;
  if (fdt_parse_aplic(fdt, &aplic_m_base, &aplic_s_base, "riscv,aplic") == 0) {
    if (aplic_m_base) {
      *base = aplic_m_base;
      return new aplic_m_t(sim, nullptr);
    }
  }
  return nullptr;
}

aplic_s_t* aplic_s_parse_from_fdt(const void* fdt, const sim_t* sim, reg_t* base, const std::vector<std::string>& sargs UNUSED)
{
  reg_t aplic_m_base, aplic_s_base;
  if (fdt_parse_aplic(fdt, &aplic_m_base, &aplic_s_base, "riscv,aplic") == 0) {
    if (aplic_s_base) {
      *base = aplic_s_base;
      auto *ptr = new aplic_s_t(sim, sim->aplic_m.get());
      if (sim->aplic_m)
        sim->aplic_m->set_child(ptr);
      return ptr;
    }
  }
  return nullptr;
}
REGISTER_DEVICE(aplic_m, aplic_m_parse_from_fdt, aplic_m_generate_dts)
REGISTER_DEVICE(aplic_s, aplic_s_parse_from_fdt, aplic_s_generate_dts)
