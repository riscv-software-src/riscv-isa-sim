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
  state->add_csr(CSR_VSTART, vstart = std::make_shared<vector_csr_t>(p, CSR_VSTART, /*mask*/ VLEN - 1));
  state->add_csr(CSR_VXRM, vxrm = std::make_shared<vector_csr_t>(p, CSR_VXRM, /*mask*/ 0x3ul));
  state->add_csr(CSR_VL, vl = std::make_shared<vector_csr_t>(p, CSR_VL, /*mask*/ 0));
  state->add_csr(CSR_VTYPE, vtype = std::make_shared<vector_csr_t>(p, CSR_VTYPE, /*mask*/ 0));
  state->add_csr(CSR_VLENB, std::make_shared<vector_csr_t>(p, CSR_VLENB, /*mask*/ 0, /*init*/ vlenb));
  assert(VCSR_VXSAT_SHIFT == 0);  // composite_csr_t assumes vxsat begins at bit 0
  state->add_csr(CSR_VCSR, std::make_shared<composite_csr_t>(p, CSR_VCSR, vxrm, state->vxsat, VCSR_VXRM_SHIFT));

  mtype = std::make_shared<vector_csr_t>(p, CSR_MTYPE, 0);
  if (TE) {
    mt.resize(16 * TE * TE);
    state->add_csr(CSR_MTYPE, mtype);
  }

  vtype->write_raw(0);
  set_vl(0, 0, 0, -1); // default to illegal configuration
}

void vectorUnit_t::msetmtype(reg_t newMType, reg_t newVType)
{
  reg_t twiden = widen = (1 << extract64(newMType, 0, 2)) >> 1;

  if (twiden) {
    int new_vlmul = log2(
      std::min(reg_t(8) / kmax(),
      std::min(reg_t(8) / twiden,
        std::max(reg_t(1), ete() / eve()))));
    newVType = (newVType & ~reg_t(7)) | new_vlmul;
    newVType |= 0xc0; // vta=vma=1;
  }

  set_vl(0, 1, 0, newVType);

  if (twiden * vsew > ELEN)
    set_vl(0, 1, 0, reg_t(-1));

  widen = vill ? 0 : twiden;
  msettk(extract64(newMType, 5, 3));
  msettm(extract64(newMType, 10, 14));
}

reg_t vectorUnit_t::read_mtype() const
{
  return
    widen |
    (tk << 5) |
    (tm << 10);
}

reg_t vectorUnit_t::msettn(reg_t atn)
{
  if (widen) {
    reg_t tn = std::min(atn, std::min(reg_t(vflmul) * eve(), ete()));
    vl->write_raw(tn);
    return tn;
  }

  return set_vl(0, 1, atn, vtype->read());
}

reg_t vectorUnit_t::msettm(reg_t atm)
{
  tm = std::min(atm, std::min(reg_t(vflmul) * eve(), ete()));
  mtype->write_raw(read_mtype());
  return tm;
}

reg_t vectorUnit_t::msettk(reg_t atk)
{
  tk = std::min(atk, kmax());
  mtype->write_raw(read_mtype());
  return tk;
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
      if (p->extension_enabled(EXT_ZVQWBDOTA8I) && vsew == 8)
        ill_altfmt = false;
      else if (p->extension_enabled(EXT_ZVQWBDOTA16I) && vsew == 16)
        ill_altfmt = false;
      else if (p->extension_enabled(EXT_ZVFQWBDOTA8F) && vsew == 8)
        ill_altfmt = false;
      else if (p->extension_enabled(EXT_ZVFWBDOTA16BF) && vsew == 16)
        ill_altfmt = false;
      else if (p->extension_enabled(EXT_ZVQWDOTA8I) && vsew == 8)
        ill_altfmt = false;
      else if (p->extension_enabled(EXT_ZVQWDOTA16I) && vsew == 16)
        ill_altfmt = false;
      else if (p->extension_enabled(EXT_ZVFQWDOTA8F) && vsew == 8)
        ill_altfmt = false;
      else if (p->extension_enabled(EXT_ZVFWDOTA16BF) && vsew == 16)
        ill_altfmt = false;
      else if (p->extension_enabled(EXT_ZVFBFA) && (vsew == 16 || vsew == 8))
        ill_altfmt = false;
      else if (p->extension_enabled(EXT_ZVFOFP8MIN) && vsew == 8)
        ill_altfmt = false;
      else if (p->extension_enabled(EXT_ZVTI8I32MM) && vsew == 8)
        ill_altfmt = false;
      else if (p->extension_enabled(EXT_ZVTOFP8FMM) && vsew == 8)
        ill_altfmt = false;
      else if (p->extension_enabled(EXT_ZVTBF16FMM) && vsew == 16)
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

  // clear mtype
  widen = 0;
  tm = 0;
  tk = 0;
  mtype->write_raw(read_mtype());

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

  return vl->read();
}

void vectorUnit_t::log_elt_write_if_needed(reg_t vReg) const {
  if (unlikely(p->get_log_commits_enabled()))
    p->get_state()->log_reg_write[((vReg) << 4) | 2] = {0, 0};
}
