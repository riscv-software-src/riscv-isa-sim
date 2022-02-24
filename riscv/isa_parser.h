// See LICENSE for license details.
#ifndef _RISCV_ISA_PARSER_H
#define _RISCV_ISA_PARSER_H

#include "decode.h"

#include <vector>
#include <string>
#include <unordered_map>

class extension_t;

typedef enum {
  // 65('A') ~ 90('Z') is reserved for standard isa in misa
  EXT_ZFH,
  EXT_ZFHMIN,
  EXT_ZBA,
  EXT_ZBB,
  EXT_ZBC,
  EXT_ZBS,
  EXT_ZBKB,
  EXT_ZBKC,
  EXT_ZBKX,
  EXT_ZKND,
  EXT_ZKNE,
  EXT_ZKNH,
  EXT_ZKSED,
  EXT_ZKSH,
  EXT_ZKR,
  EXT_ZMMUL,
  EXT_ZBPBO,
  EXT_ZPN,
  EXT_ZPSFOPERAND,
  EXT_SVNAPOT,
  EXT_SVPBMT,
  EXT_SVINVAL,
  EXT_ZDINX,
  EXT_ZFINX,
  EXT_ZHINX,
  EXT_ZHINXMIN,
  EXT_ZICBOM,
  EXT_ZICBOZ,
  EXT_ZICNTR,
  EXT_ZIHPM,
  EXT_XZBP,
  EXT_XZBS,
  EXT_XZBE,
  EXT_XZBF,
  EXT_XZBC,
  EXT_XZBM,
  EXT_XZBR,
  EXT_XZBT,
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
  ~isa_parser_t(){};
  unsigned get_max_xlen() const { return max_xlen; }
  reg_t get_max_isa() const { return max_isa; }
  std::string get_isa_string() const { return isa_string; }
  bool extension_enabled(unsigned char ext) const {
    if (ext >= 'A' && ext <= 'Z')
      return (max_isa >> (ext - 'A')) & 1;
    else
      return extension_table[ext];
  }
  const std::unordered_map<std::string, extension_t*> &
  get_extensions() const { return extensions; }

protected:
  unsigned max_xlen;
  reg_t max_isa;
  std::vector<bool> extension_table;
  std::string isa_string;
  std::unordered_map<std::string, extension_t*> extensions;
};

#endif
