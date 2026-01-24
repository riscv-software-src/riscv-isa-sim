// #include "extension_cswprof.h"
// #include "csrs.h"

// #define CSR_CSW_COUNT      0x7C0
// #define CSR_CSW_LASTCAUSE  0x7C1

// cswprof_extension_t::cswprof_extension_t() {}

// std::vector<insn_desc_t>
// cswprof_extension_t::get_instructions(const processor_t&)
// {
//   return {};   // No custom instructions
// }

// std::vector<disasm_insn_t*>
// cswprof_extension_t::get_disasms(const processor_t*)
// {
//   return {};   // No disassembler extensions
// }

// void cswprof_extension_t::install_csrs(processor_t* p)
// {
//   proc = p;
//   csw_count = 0;
//   csw_last_cause = 0;

//   auto& csrmap = proc->get_state()->csrmap;

//   csrmap[CSR_CSW_COUNT] =
//     std::make_shared<basic_csr_t>(proc, CSR_CSW_COUNT, 0);

//   csrmap[CSR_CSW_LASTCAUSE] =
//     std::make_shared<basic_csr_t>(proc, CSR_CSW_LASTCAUSE, 0);
// }

#include "extension_cswprof.h"

REGISTER_EXTENSION(cswprof, []() { return new cswprof_extension_t(); })

std::vector<csr_t_p> cswprof_extension_t::get_csrs(processor_t& proc) const {
  std::vector<csr_t_p> v;

  v.push_back(std::make_shared<basic_csr_t>(&proc, CSR_CSW_COUNT, 0));
  v.push_back(std::make_shared<basic_csr_t>(&proc, CSR_CSW_LASTCAUSE, 0));
  v.push_back(std::make_shared<basic_csr_t>(&proc, CSR_CSW_CYCLES, 0));
  v.push_back(std::make_shared<basic_csr_t>(&proc, CSR_CSW_LAST_TID, 0));

  return v;
}
