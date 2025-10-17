// See LICENSE for license details

#include "config.h"
#include "vector_unit.h"
#include "processor.h"
#include "arith.h"

void vectorUnit_t::vectorUnit_t::reset()
{
  free(reg_file);
  VLEN = get_vlen();
  ELEN = get_elen();
  reg_file = malloc(NVPR * vlenb);
  memset(reg_file, 0, NVPR * vlenb);

  auto state = p->get_state();
  state->add_csr(CSR_VXSAT, vxsat = std::make_shared<vxsat_csr_t>(p, CSR_VXSAT));
  state->add_csr(CSR_VSTART, vstart = std::make_shared<vector_csr_t>(p, CSR_VSTART, /*mask*/ VLEN - 1));
  state->add_csr(CSR_VXRM, vxrm = std::make_shared<vector_csr_t>(p, CSR_VXRM, /*mask*/ 0x3ul));
  state->add_csr(CSR_VL, vl = std::make_shared<vector_csr_t>(p, CSR_VL, /*mask*/ 0));
  state->add_csr(CSR_VTYPE, vtype = std::make_shared<vector_csr_t>(p, CSR_VTYPE, /*mask*/ 0));
  state->add_csr(CSR_VLENB, std::make_shared<vector_csr_t>(p, CSR_VLENB, /*mask*/ 0, /*init*/ vlenb));
  assert(VCSR_VXSAT_SHIFT == 0);  // composite_csr_t assumes vxsat begins at bit 0
  state->add_csr(CSR_VCSR, std::make_shared<composite_csr_t>(p, CSR_VCSR, vxrm, vxsat, VCSR_VXRM_SHIFT));

  vtype->write_raw(0);
  set_vl(0, 0, 0, -1); // default to illegal configuration
}

reg_t vectorUnit_t::vectorUnit_t::set_vl(int rd, int rs1, reg_t reqVL, reg_t newType)
{
  if (vtype->read() != newType) {
    int new_vlmul = int8_t(extract64(newType, 0, 3) << 5) >> 5;
    auto old_vlmax = vlmax;

    vsew = 1 << (extract64(newType, 3, 3) + 3);
    vflmul = new_vlmul >= 0 ? 1 << new_vlmul : 1.0 / (1 << -new_vlmul);
    vlmax = (VLEN/vsew) * vflmul;
    vta = extract64(newType, 6, 1);
    vma = extract64(newType, 7, 1);
    altfmt = extract64(newType, 8, 1);

    bool ill_altfmt = true;
    if (altfmt) {
      if (p->extension_enabled(EXT_ZVQBDOT8I) && vsew == 8)
        ill_altfmt = false;
      else if (p->extension_enabled(EXT_ZVQBDOT16I) && vsew == 16)
        ill_altfmt = false;
      else if (p->extension_enabled(EXT_ZVFQBDOT8F) && vsew == 8)
        ill_altfmt = false;
      else if (p->extension_enabled(EXT_ZVFWBDOT16BF) && vsew == 16)
        ill_altfmt = false;
      else if (p->extension_enabled(EXT_ZVQLDOT8I) && vsew == 8)
        ill_altfmt = false;
      else if (p->extension_enabled(EXT_ZVQLDOT16I) && vsew == 16)
        ill_altfmt = false;
      else if (p->extension_enabled(EXT_ZVFQLDOT8F) && vsew == 8)
        ill_altfmt = false;
      else if (p->extension_enabled(EXT_ZVFWLDOT16BF) && vsew == 16)
        ill_altfmt = false;
      else if (p->extension_enabled(EXT_ZVFBFA) && (vsew == 16 || vsew == 8))
        ill_altfmt = false;
      else if (p->extension_enabled(EXT_ZVFOFP8MIN) && vsew == 8)
        ill_altfmt = false;
    }

    vill = !(vflmul >= 0.125 && vflmul <= 8)
           || vsew > std::min(vflmul, 1.0f) * ELEN
           || (newType >> 9) != 0
           || (altfmt && ill_altfmt)
           || (rd == 0 && rs1 == 0 && old_vlmax != vlmax);

    if (vill) {
      vlmax = 0;
      vtype->write_raw(UINT64_MAX << (p->get_xlen() - 1));
    } else {
      vtype->write_raw(newType);
    }
  }

  // set vl
  if (vlmax == 0) {
    vl->write_raw(0);
  } else if (rd == 0 && rs1 == 0) {
    ; // retain current VL
  } else if (rd != 0 && rs1 == 0) {
    vl->write_raw(vlmax);
  } else if (rs1 != 0) {
    vl->write_raw(std::min(reqVL, vlmax));
  }

  vstart->write_raw(0);
  return vl->read();
}

void vectorUnit_t::log_elt_write_if_needed(reg_t vReg) const {
  if (unlikely(p->get_log_commits_enabled()))
    p->get_state()->log_reg_write[((vReg) << 4) | 2] = {0, 0};
}
