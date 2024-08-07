// See LICENSE for license details.
#ifndef _RISCV_ISA_PARSER_H
#define _RISCV_ISA_PARSER_H

#include "decode.h"

#include <bitset>
#include <string>
#include <set>

typedef enum {
  // 65('A') ~ 90('Z') is reserved for standard isa in misa
  EXT_ZFH = 'Z' + 1,
  EXT_ZFHMIN,
  EXT_ZBA,
  EXT_ZBB,
  EXT_ZBC,
  EXT_ZBS,
  EXT_ZBKB,
  EXT_ZBKC,
  EXT_ZBKX,
  EXT_ZCA,
  EXT_ZCB,
  EXT_ZCD,
  EXT_ZCF,
  EXT_ZCMLSD,
  EXT_ZCMP,
  EXT_ZCMT,
  EXT_ZKND,
  EXT_ZKNE,
  EXT_ZKNH,
  EXT_ZKSED,
  EXT_ZKSH,
  EXT_ZKR,
  EXT_ZMMUL,
  EXT_ZVFH,
  EXT_ZVFHMIN,
  EXT_SMEPMP,
  EXT_SMSTATEEN,
  EXT_SMRNMI,
  EXT_SSCOFPMF,
  EXT_SVADU,
  EXT_SVNAPOT,
  EXT_SVPBMT,
  EXT_SVINVAL,
  EXT_ZDINX,
  EXT_ZFA,
  EXT_ZFBFMIN,
  EXT_ZFINX,
  EXT_ZHINX,
  EXT_ZHINXMIN,
  EXT_ZICBOM,
  EXT_ZICBOZ,
  EXT_ZICNTR,
  EXT_ZICOND,
  EXT_ZIHPM,
  EXT_ZILSD,
  EXT_ZVBB,
  EXT_ZVBC,
  EXT_ZVFBFMIN,
  EXT_ZVFBFWMA,
  EXT_ZVKG,
  EXT_ZVKNED,
  EXT_ZVKNHA,
  EXT_ZVKNHB,
  EXT_ZVKSED,
  EXT_ZVKSH,
  EXT_SSTC,
  EXT_ZAAMO,
  EXT_ZALRSC,
  EXT_ZACAS,
  EXT_ZABHA,
  EXT_ZAWRS,
  EXT_INTERNAL_ZFH_MOVE,
  EXT_SMCSRIND,
  EXT_SSCSRIND,
  EXT_SMCNTRPMF,
  EXT_ZIMOP,
  EXT_ZCMOP,
  EXT_ZALASR,
  EXT_SSQOSID,
  EXT_ZICFILP,
  EXT_ZICFISS,
  EXT_SSDBLTRP,
  EXT_SMDBLTRP,
  EXT_SMMPM,
  EXT_SMNPM,
  EXT_SSNPM,
  NUM_ISA_EXTENSIONS
} isa_extension_t;

typedef enum {
  IMPL_MMU_SV32,
  IMPL_MMU_SV39,
  IMPL_MMU_SV48,
  IMPL_MMU_SV57,
  IMPL_MMU_SBARE,
  IMPL_MMU,
  IMPL_MMU_VMID,
  IMPL_MMU_ASID,
} impl_extension_t;

class isa_parser_t {
public:
  isa_parser_t(const char* str, const char *priv);
  ~isa_parser_t() {};
  unsigned get_max_xlen() const { return max_xlen; }
  reg_t get_max_isa() const { return max_isa; }
  std::string get_isa_string() const { return isa_string; }
  reg_t get_vlen() const { return vlen; }
  reg_t get_elen() const { return elen; }
  bool get_zvf() const { return zvf; }
  bool get_zvd() const { return zvd; }
  bool extension_enabled(unsigned char ext) const {
    return extension_enabled(isa_extension_t(ext));
  }
  bool extension_enabled(isa_extension_t ext) const {
    return extension_table[ext];
  }
  bool has_any_vector() const { return vlen > 0; }

  std::bitset<NUM_ISA_EXTENSIONS> get_extension_table() const { return extension_table; }

  const std::set<std::string> &get_extensions() const { return extensions; }

protected:
  unsigned max_xlen;
  reg_t max_isa;
  reg_t vlen;
  reg_t elen;
  bool zvf;
  bool zvd;
  std::bitset<NUM_ISA_EXTENSIONS> extension_table;
  std::string isa_string;
  std::set<std::string> extensions;
};

#endif
