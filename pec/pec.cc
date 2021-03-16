#include "extension.h"
#include <cstring>

#include "qarma.h"
#include "pec-opcode.h"

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return xpr_name[insn.rs1()];
  }
} xrs1;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return xpr_name[insn.rs2()];
  }
} xrs2;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return xpr_name[insn.rd()];
  }
} xrd;


static reg_t pec_encryption(processor_t* p, insn_t insn, reg_t pc)
{
  require_privilege(PRV_S);
  reg_t w0 = 0, k0 = 0;

  if ((insn.bits() & MASK_CRETK) == MATCH_CRETK) {
    w0 = p->get_csr(CSR_SCRTKEYH);
    k0 = p->get_csr(CSR_SCRTKEYL);
  } else if ((insn.bits() & MASK_CREAK) == MATCH_CREAK) {
    w0 = p->get_csr(CSR_SCRAKEYH);
    k0 = p->get_csr(CSR_SCRAKEYL);
  } else if ((insn.bits() & MASK_CREBK) == MATCH_CREBK) {
    w0 = p->get_csr(CSR_SCRBKEYH);
    k0 = p->get_csr(CSR_SCRBKEYL);
  } else if ((insn.bits() & MASK_CREMK) == MATCH_CREMK) {
    w0 = p->get_csr(CSR_MCRMKEYH);
    k0 = p->get_csr(CSR_MCRMKEYL);
  } else {
    throw std::runtime_error("bad pec instruction");
  }

  reg_t result = qarma64_enc(RS1, RS2, w0, k0, 7);
  WRITE_RD(result);
  return pc + 4;
}

static reg_t pec_decryption(processor_t* p, insn_t insn, reg_t pc)
{
  require_privilege(PRV_S);

  reg_t w0 = 0, k0 = 0;

  if ((insn.bits() & MASK_CRDTK) == MATCH_CRDTK) {
    w0 = p->get_csr(CSR_SCRTKEYH);
    k0 = p->get_csr(CSR_SCRTKEYL);
  } else if ((insn.bits() & MASK_CRDAK) == MATCH_CRDAK) {
    w0 = p->get_csr(CSR_SCRAKEYH);
    k0 = p->get_csr(CSR_SCRAKEYL);
  } else if ((insn.bits() & MASK_CRDBK) == MATCH_CRDBK) {
    w0 = p->get_csr(CSR_SCRBKEYH);
    k0 = p->get_csr(CSR_SCRBKEYL);
  } else if ((insn.bits() & MASK_CRDMK) == MATCH_CRDMK) {
    w0 = p->get_csr(CSR_MCRMKEYH);
    k0 = p->get_csr(CSR_MCRMKEYL);
  } else {
    throw std::runtime_error("bad pec instruction");
  }

  reg_t result = qarma64_dec(RS1, RS2, w0, k0, 7);
  WRITE_RD(result);
  return pc + 4;
}

class pec_t : public extension_t
{
 public:
  const char* name() { return "pec"; }

  pec_t() {
    printf("create pec class\n");
  }

  std::vector<insn_desc_t> get_instructions() {
    std::vector<insn_desc_t> insns;

    insns.push_back((insn_desc_t){MATCH_CRETK, MASK_CRETK, pec_encryption, pec_encryption}); 
    insns.push_back((insn_desc_t){MATCH_CRDTK, MASK_CRDTK, pec_decryption, pec_decryption});
    insns.push_back((insn_desc_t){MATCH_CREMK, MASK_CREMK, pec_encryption, pec_encryption}); 
    insns.push_back((insn_desc_t){MATCH_CRDMK, MASK_CRDMK, pec_decryption, pec_decryption});
    insns.push_back((insn_desc_t){MATCH_CREAK, MASK_CREAK, pec_encryption, pec_encryption}); 
    insns.push_back((insn_desc_t){MATCH_CRDAK, MASK_CRDAK, pec_decryption, pec_decryption});
    insns.push_back((insn_desc_t){MATCH_CREBK, MASK_CREBK, pec_encryption, pec_encryption}); 
    insns.push_back((insn_desc_t){MATCH_CRDBK, MASK_CRDBK, pec_decryption, pec_decryption});

    return insns;
  }

  std::vector<disasm_insn_t*> get_disasms() {
    std::vector<disasm_insn_t*> insns;

    insns.push_back(new disasm_insn_t("cretk", MATCH_CRETK, MASK_CRETK, {&xrd, &xrs1, &xrs2}));
    insns.push_back(new disasm_insn_t("crdtk", MATCH_CRDTK, MASK_CRDTK, {&xrd, &xrs1, &xrs2}));
    insns.push_back(new disasm_insn_t("cremk", MATCH_CREMK, MASK_CREMK, {&xrd, &xrs1, &xrs2}));
    insns.push_back(new disasm_insn_t("crdmk", MATCH_CRDMK, MASK_CRDMK, {&xrd, &xrs1, &xrs2}));
    insns.push_back(new disasm_insn_t("creak", MATCH_CREAK, MASK_CREAK, {&xrd, &xrs1, &xrs2}));
    insns.push_back(new disasm_insn_t("crdak", MATCH_CRDAK, MASK_CRDAK, {&xrd, &xrs1, &xrs2}));
    insns.push_back(new disasm_insn_t("crebk", MATCH_CREBK, MASK_CREBK, {&xrd, &xrs1, &xrs2}));
    insns.push_back(new disasm_insn_t("crdbk", MATCH_CRDBK, MASK_CRDBK, {&xrd, &xrs1, &xrs2}));

    return insns;
  }
};

REGISTER_EXTENSION(pec, []() { return new pec_t; })
