// vmpopc rd, vs2, vm
require(P.VU.vsew >= e8 && P.VU.vsew <= e64);
require_vector(true);
reg_t sew = P.VU.vsew;
reg_t rd_num = insn.rd();
require_align(rd_num, P.VU.vflmul);
require_vm;

V_EXT_VSTART_CHECK;
for (reg_t i = P.VU.vstart->read() ; i < std::max(P.VU.vlmax, P.VU.VLEN/P.VU.vsew); ++i) {
  VI_LOOP_ELEMENT_SKIP();
  reg_t vl = P.VU.vl->read();

  if (0 == P.VU.vta && i >= vl) { \
    continue; \
  } \
  if ((true == skip && 1 == P.VU.vma && i < vl) || (1 == P.VU.vta && i >= vl)) \
    mata_action = 2; \
  else \
    mata_action = 1; \
  switch (sew) {
  case e8:
    if (1 == mata_action)
      P.VU.elt<uint8_t>(rd_num, i, true) = i;
    else
      P.VU.elt<uint8_t>(rd_num, i, true) = 0xFF;
    break;
  case e16:
    if (1 == mata_action)
      P.VU.elt<uint16_t>(rd_num, i, true) = i;
    else
      P.VU.elt<uint16_t>(rd_num, i, true) = 0xFFFF;
    break;
  case e32:
    if (1 == mata_action)
      P.VU.elt<uint32_t>(rd_num, i, true) = i;
    else
      P.VU.elt<uint32_t>(rd_num, i, true) = 0xFFFFFFFF;
    break;
  default:
    if (1 == mata_action)
      P.VU.elt<uint64_t>(rd_num, i, true) = i;
    else
      P.VU.elt<uint64_t>(rd_num, i, true) = 0xFFFFFFFFFFFFFFFF;
    break;
  }
}

P.VU.vstart->write(0);
