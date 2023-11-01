// vmpopc rd, vs2, vm
require(P.VU.vsew >= e8 && P.VU.vsew <= e64);
require_vector(true);
reg_t vl = P.VU.vl->read();
reg_t sew = P.VU.vsew;
reg_t rd_num = insn.rd();
reg_t rs2_num = insn.rs2();
require(P.VU.vstart->read() == 0);
require_vm;
require_align(rd_num, P.VU.vflmul);
require_noover(rd_num, P.VU.vflmul, rs2_num, 1);

int cnt = 0;
V_EXT_VSTART_CHECK;
for (reg_t i = 0; i < std::max(P.VU.vlmax, P.VU.VLEN/P.VU.vsew); ++i) {
  const int midx = i / 64;
  const int mpos = i % 64;
  int mata_action = 0; //0:origin, 1:calculate, 2:pad FF

  bool vs2_lsb = ((P.VU.elt<uint64_t>(rs2_num, midx) >> mpos) & 0x1) == 1;
  bool do_mask = (P.VU.elt<uint64_t>(0, midx) >> mpos) & 0x1;

  bool has_one = false;
  if (insn.v_vm() == 1 || (insn.v_vm() == 0 && do_mask)) {
    if (vs2_lsb) {
      has_one = true;
    }
  }

  bool use_ori = (insn.v_vm() == 0) && !do_mask;
  if (0 == P.VU.vta && i >= vl) { \
    continue; \
  } \
  if ((1 == P.VU.vta && i >= vl) || (insn.v_vm() == 0 && 1 == P.VU.vma && !do_mask && i < vl)) \
    mata_action = 2; \
  else \
    mata_action = 1; \
  switch (sew) {
  case e8:
    if (1 == mata_action) {
      P.VU.elt<uint8_t>(rd_num, i, true) = use_ori ?
                                     P.VU.elt<uint8_t>(rd_num, i) : cnt;
    } else \
				P.VU.elt<uint8_t>(rd_num, i, true) = 0xFF;
    break;
  case e16:
    if (1 == mata_action) {
      P.VU.elt<uint16_t>(rd_num, i, true) = use_ori ?
                                      P.VU.elt<uint16_t>(rd_num, i) : cnt;
    } else \
				P.VU.elt<uint16_t>(rd_num, i, true) = 0xFFFF;
    break;
  case e32:
    if (1 == mata_action) {
      P.VU.elt<uint32_t>(rd_num, i, true) = use_ori ?
                                      P.VU.elt<uint32_t>(rd_num, i) : cnt;
    } else \
				P.VU.elt<uint32_t>(rd_num, i, true) = 0xFFFFFFFF;
    break;
  default:
    if (1 == mata_action) {
      P.VU.elt<uint64_t>(rd_num, i, true) = use_ori ?
                                      P.VU.elt<uint64_t>(rd_num, i) : cnt;
    } else \
      P.VU.elt<uint64_t>(rd_num, i, true) = 0xFFFFFFFFFFFFFFFF;
    break;
  }

  if (has_one) {
    cnt++;
  }
}

