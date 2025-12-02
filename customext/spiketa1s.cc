#include "agnostic_macros.h"
#include "extension.h"
#include "processor.h"

#include <algorithm>
#include <cmath>
#include <numeric>

static bool is_mask_dest(insn_t insn) {
  const auto &bits = insn.bits();
  return (bits & MASK_VMAND_MM) == MATCH_VMAND_MM ||
         (bits & MASK_VMNAND_MM) == MATCH_VMNAND_MM ||
         (bits & MASK_VMANDN_MM) == MATCH_VMANDN_MM ||
         (bits & MASK_VMXOR_MM) == MATCH_VMXOR_MM ||
         (bits & MASK_VMOR_MM) == MATCH_VMOR_MM ||
         (bits & MASK_VMNOR_MM) == MATCH_VMNOR_MM ||
         (bits & MASK_VMORN_MM) == MATCH_VMORN_MM ||
         (bits & MASK_VMXNOR_MM) == MATCH_VMXNOR_MM ||
         (bits & MASK_VLM_V) == MATCH_VLM_V ||
         // rvv-spec-1.0: vmsbf.m, vmsif.m, vmsof.m: The tail elements in the
         // destination mask register are updated under a tail-agnostic policy.
         (bits & MASK_VMSBF_M) == MATCH_VMSBF_M ||
         (bits & MASK_VMSIF_M) == MATCH_VMSIF_M ||
         (bits & MASK_VMSOF_M) == MATCH_VMSOF_M ||
         // rvv-spec-1.0: Vector Integer Add-with-Carry / Subtract-with-Borrow
         // Instructions: Because these instructions produce a mask value, they
         // always operate with a tail-agnostic policy.
         (bits & MASK_VADC_VVM) == MATCH_VADC_VVM ||
         (bits & MASK_VADC_VXM) == MATCH_VADC_VXM ||
         (bits & MASK_VADC_VIM) == MATCH_VADC_VIM ||
         (bits & MASK_VMADC_VVM) == MATCH_VMADC_VVM ||
         (bits & MASK_VMADC_VXM) == MATCH_VMADC_VXM ||
         (bits & MASK_VMADC_VIM) == MATCH_VMADC_VIM ||
         (bits & MASK_VMADC_VV) == MATCH_VMADC_VV ||
         (bits & MASK_VMADC_VX) == MATCH_VMADC_VX ||
         (bits & MASK_VMADC_VI) == MATCH_VMADC_VI ||
         (bits & MASK_VSBC_VVM) == MATCH_VSBC_VVM ||
         (bits & MASK_VSBC_VXM) == MATCH_VSBC_VXM ||
         (bits & MASK_VMSBC_VVM) == MATCH_VMSBC_VVM ||
         (bits & MASK_VMSBC_VXM) == MATCH_VMSBC_VXM ||
         (bits & MASK_VMSBC_VV) == MATCH_VMSBC_VV ||
         (bits & MASK_VMSBC_VX) == MATCH_VMSBC_VX ||
         // rvv-spec-1.0: Vector Integer Compare Instructions: The destination
         // mask vector is always held in a single vector register, with a
         // layout of elements as described in Section Mask Register Layout.
         (bits & MASK_VMSEQ_VV) == MATCH_VMSEQ_VV ||
         (bits & MASK_VMSEQ_VX) == MATCH_VMSEQ_VX ||
         (bits & MASK_VMSEQ_VI) == MATCH_VMSEQ_VI ||
         (bits & MASK_VMSNE_VV) == MATCH_VMSNE_VV ||
         (bits & MASK_VMSNE_VX) == MATCH_VMSNE_VX ||
         (bits & MASK_VMSNE_VI) == MATCH_VMSNE_VI ||
         (bits & MASK_VMSLTU_VV) == MATCH_VMSLTU_VV ||
         (bits & MASK_VMSLTU_VX) == MATCH_VMSLTU_VX ||
         (bits & MASK_VMSLT_VV) == MATCH_VMSLT_VV ||
         (bits & MASK_VMSLT_VX) == MATCH_VMSLT_VX ||
         (bits & MASK_VMSLEU_VV) == MATCH_VMSLEU_VV ||
         (bits & MASK_VMSLEU_VX) == MATCH_VMSLEU_VX ||
         (bits & MASK_VMSLEU_VI) == MATCH_VMSLEU_VI ||
         (bits & MASK_VMSLE_VV) == MATCH_VMSLE_VV ||
         (bits & MASK_VMSLE_VX) == MATCH_VMSLE_VX ||
         (bits & MASK_VMSLE_VI) == MATCH_VMSLE_VI ||
         (bits & MASK_VMSGTU_VX) == MATCH_VMSGTU_VX ||
         (bits & MASK_VMSGTU_VI) == MATCH_VMSGTU_VI ||
         (bits & MASK_VMSGT_VX) == MATCH_VMSGT_VX ||
         (bits & MASK_VMSGT_VI) == MATCH_VMSGT_VI ||
         // rvv-spec-1.0: Vector Floating-Point Compare Instructions: Compares
         // write mask registers, and so always operate under a tail-agnostic
         // policy.
         (bits & MASK_VMFEQ_VV) == MATCH_VMFEQ_VV ||
         (bits & MASK_VMFEQ_VF) == MATCH_VMFEQ_VF ||
         (bits & MASK_VMFNE_VV) == MATCH_VMFNE_VV ||
         (bits & MASK_VMFNE_VF) == MATCH_VMFNE_VF ||
         (bits & MASK_VMFLT_VV) == MATCH_VMFLT_VV ||
         (bits & MASK_VMFLT_VF) == MATCH_VMFLT_VF ||
         (bits & MASK_VMFLE_VV) == MATCH_VMFLE_VV ||
         (bits & MASK_VMFLE_VF) == MATCH_VMFLE_VF ||
         (bits & MASK_VMFGT_VF) == MATCH_VMFGT_VF ||
         (bits & MASK_VMFGE_VF) == MATCH_VMFGE_VF;
}

static bool is_single_dest(insn_t insn) {
  const auto &bits = insn.bits();
  return (bits & MASK_VMV_S_X) == MATCH_VMV_S_X ||
         (bits & MASK_VFMV_S_F) == MATCH_VFMV_S_F;
}

static reg_t get_instr_evl(const processor_t *p, insn_t insn) {
  // There is no accounting for whole register instructions here, as they do not
  // have a tail.
  reg_t vl = p->VU.vl->read();
  // rvv-spec-1.0: vlm.v: These operate similarly to unmasked byte loads or
  // stores (EEW=8), except that the effective vector length is evl=vl/8.
  if ((insn.bits() & MASK_VLM_V) == MATCH_VLM_V)
    return vl / 8;
  return vl;
}

static reg_t get_data_eew(const processor_t *p, insn_t insn) {
  assert(is_rvv_or_fp_load(insn));
  const auto &bits = insn.bits();
  // rvv-spec-1.0: vector indexed loads and stores use the EEW/EMUL encoded in
  // the instruction for the index values and the SEW/LMUL encoded in vtype for
  // the data values
  if ((bits & MASK_VLOXEI16_V) == MATCH_VLOXEI16_V ||
      (bits & MASK_VLOXEI32_V) == MATCH_VLOXEI32_V ||
      (bits & MASK_VLOXEI64_V) == MATCH_VLOXEI64_V ||
      (bits & MASK_VLOXEI8_V) == MATCH_VLOXEI8_V ||
      (bits & MASK_VLUXEI16_V) == MATCH_VLUXEI16_V ||
      (bits & MASK_VLUXEI32_V) == MATCH_VLUXEI32_V ||
      (bits & MASK_VLUXEI64_V) == MATCH_VLUXEI64_V ||
      (bits & MASK_VLUXEI8_V) == MATCH_VLUXEI8_V)
    return p->VU.vsew;
  // rvv-spec-1.0: Vector unit-stride and constant-stride use the EEW/EMUL
  // encoded in the instruction for the data values
  switch (insn.v_width()) {
  case 0b000:
    return e8;
  case 0b101:
    return e16;
  case 0b110:
    return e32;
  case 0b111:
    return e64;
  }
  assert(0);
}

static bool is_whole_reg(insn_t insn) {
  const auto &bits = insn.bits();
  return (bits & MASK_VMV1R_V) == MATCH_VMV1R_V ||
         (bits & MASK_VMV2R_V) == MATCH_VMV2R_V ||
         (bits & MASK_VMV4R_V) == MATCH_VMV4R_V ||
         (bits & MASK_VMV8R_V) == MATCH_VMV8R_V ||
         (bits & MASK_VL1RE8_V) == MATCH_VL1RE8_V ||
         (bits & MASK_VL2RE8_V) == MATCH_VL2RE8_V ||
         (bits & MASK_VL4RE8_V) == MATCH_VL4RE8_V ||
         (bits & MASK_VL8RE8_V) == MATCH_VL8RE8_V ||
         (bits & MASK_VL1RE16_V) == MATCH_VL1RE16_V ||
         (bits & MASK_VL2RE16_V) == MATCH_VL2RE16_V ||
         (bits & MASK_VL4RE16_V) == MATCH_VL4RE16_V ||
         (bits & MASK_VL8RE16_V) == MATCH_VL8RE16_V ||
         (bits & MASK_VL1RE32_V) == MATCH_VL1RE32_V ||
         (bits & MASK_VL2RE32_V) == MATCH_VL2RE32_V ||
         (bits & MASK_VL4RE32_V) == MATCH_VL4RE32_V ||
         (bits & MASK_VL8RE32_V) == MATCH_VL8RE32_V ||
         (bits & MASK_VL1RE64_V) == MATCH_VL1RE64_V ||
         (bits & MASK_VL2RE64_V) == MATCH_VL2RE64_V ||
         (bits & MASK_VL4RE64_V) == MATCH_VL4RE64_V ||
         (bits & MASK_VL8RE64_V) == MATCH_VL8RE64_V;
}

static reg_t tail_agnostic_fill1s(processor_t *p, insn_t insn, reg_t pc) {
  // rvv-spec-1.0: Vector Loads and Stores: Masked vector stores only update
  // active memory elements.
  if (is_rvv_or_fp_store(insn))
    return pc;
  // Belonging to the V extension is checked after loads and stores, because
  // their opcodes do not match the opcodes of V extension.
  if (!is_rvv(insn) || is_rvv_scalar_dest(insn))
    return pc;
  // Whole register instructions don't have a tail.
  if (is_whole_reg(insn))
    return pc;
  // If the instruction has evl, then - rvv-spec-1.0: The usual property that
  // no elements are written if vstart ≥ vl does not apply to these
  // instructions. Instead, no elements are written if vstart ≥ evl.
  reg_t evl = get_instr_evl(p, insn);
  // rvv-spec-1.0: When vstart ≥ vl, there are no body elements, and no
  // elements are updated in any destination vector register group, including
  // that no tail elements are updated with agnostic values. As a consequence,
  // when vl=0, no elements, including agnostic elements, are updated in the
  // destination vector register group regardless of vstart.
  // FIXME: Vstart is used here, which was reset to 0 as a result of executing
  // this instruction, we need to get the old value of Vstart here.
  if (p->VU.vstart->read() >= evl || evl == 0)
    return pc;

  reg_t tail_from = evl;
  reg_t vl_one_reg = p->VU.VLEN / p->VU.vsew;
  reg_t tail_to = std::max(p->VU.vlmax, vl_one_reg);
  if (is_rvv_or_fp_load(insn)) {
    const reg_t nf = insn.v_nf() + 1;
    const reg_t eew = get_data_eew(p, insn);
    if ((insn.bits() & MASK_VLM_V) == MATCH_VLM_V) {
      AGNOSTIC_LOOP_LOAD_TAIL(
          { // tail agnostic - fill 1s
            vd = all_ones;
          },
          eew, nf, tail_from, p->VU.VLEN / eew);
      return pc;
    }
    vl_one_reg = p->VU.VLEN / eew;
    tail_to = std::max(p->VU.vlmax, vl_one_reg);
    AGNOSTIC_LOOP_LOAD_TAIL(
        { // tail agnostic - fill 1s
          vd = all_ones;
        },
        eew, nf, tail_from, tail_to);
    return pc;
  }
  // rvv-spec-1.0: mask destination tail elements are always treated as
  // tail-agnostic, regardless of the setting of vta.
  if (is_mask_dest(insn)) {
    AGNOSTIC_LOOP_MASK_TAIL(
        // tail agnostic - fill 1s
        /* value */ 1, p->VU.vl->read(), p->VU.VLEN);
    return pc;
  }
  // When vta==0 then undisturbed policy is in effect.
  if (p->VU.vta == 0)
    return pc;

  // rvv-spec-1.0: vmv.s.x, vfmv.s.f: The instructions ignore LMUL and vector
  // register groups; The other elements in the destination vector register
  // ( 0 < index < VLEN/SEW) are treated as tail elements using the current tail
  // agnostic/undisturbed policy.
  if (is_single_dest(insn)) {
    tail_from = 1;
    tail_to = vl_one_reg;
  }
  // Special way to select the tail elements of the vcompress.vm.
  else if ((insn.bits() & MASK_VCOMPRESS_VM) == MATCH_VCOMPRESS_VM) {
    // It is safe to read vs1 after instruction execution, since rvv-spec-1.0:
    // vcompress.vm: The destination vector register group cannot overlap the
    // source vector register group or the source mask register.
    std::vector<reg_t> idxes(evl);
    std::iota(idxes.begin(), idxes.end(), /*value*/ 0);
    tail_from = std::count_if(idxes.begin(), idxes.end(), [&](reg_t i) {
      return p->VU.mask_elt(insn.rs1(), i);
    });
  }

  AGNOSTIC_LOOP_TAIL(
      // tail agnostic - fill 1s
      { vd = all_ones; }, tail_from, tail_to);
  return pc;
}

struct tail_agnostic_fill1s_t : public extension_t {
  const char *name() const override { return "spiketa1s"; }

  tail_agnostic_fill1s_t() {}

  std::vector<insn_desc_t> get_instructions(const processor_t &) override {
    return {};
  }

  std::vector<disasm_insn_t *> get_disasms(const processor_t *) override {
    return {};
  }

  void reset(processor_t &p) override {
    auto &insn_postprocesses = p.get_state()->insn_postprocesses;
    auto tail_func = std::find(insn_postprocesses.begin(),
                               insn_postprocesses.end(), &tail_agnostic_fill1s);
    if (tail_func == insn_postprocesses.end())
      insn_postprocesses.push_back(&tail_agnostic_fill1s);
  }
};

REGISTER_EXTENSION(spiketa1s, []() {
  static tail_agnostic_fill1s_t ext;
  return &ext;
})
