// #pragma once
// #include "extension.h"
// #include "processor.h"

// // ---- CSW PROFILER CSR ADDRESSES ----
// #define CSR_CSW_COUNT      0x7C0
// #define CSR_CSW_LASTCAUSE  0x7C1
// // -----------------------------------


// class cswprof_extension_t : public extension_t
// {
// public:
//   cswprof_extension_t();

//   const char* name() const override { return "cswprof"; }

//   // REQUIRED by extension_t
//   std::vector<insn_desc_t> get_instructions(const processor_t&) override;
//   std::vector<disasm_insn_t*> get_disasms(const processor_t* = nullptr) override;

//   void install_csrs(processor_t* p);

//   uint64_t csw_count = 0;
//   uint64_t csw_last_cause = 0;

// private:
//   processor_t* proc = nullptr;
// };

#pragma once
#include "extension.h"
#include "csrs.h"   // for basic_csr_t, csr_t_p
#include "encoding.h"
#include <vector>
#include <memory>

#define CSR_CSW_COUNT      0x7C0
#define CSR_CSW_LASTCAUSE  0x7C1
#define CSR_CSW_CYCLES     0x7C2
#define CSR_CSW_LAST_TID   0x7C3

class cswprof_extension_t : public extension_t {
public:
  // MUST match base signatures
  const char* name() const override { return "cswprof"; }

  std::vector<insn_desc_t> get_instructions(const processor_t&) override {
    return {}; // no custom instructions for now
  }

  std::vector<disasm_insn_t*> get_disasms(const processor_t* = nullptr) override {
    return {};
  }

  std::vector<csr_t_p> get_csrs(processor_t& proc) const override;

  // state used for milestone 2
  mutable reg_t last_cycle = 0;
};
