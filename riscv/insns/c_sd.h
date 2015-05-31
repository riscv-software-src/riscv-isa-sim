require_extension('C');
if (xlen == 32) {
  int32_t res;
  switch ((insn.bits() >> 10) & 7) {
    case 0:
      switch ((insn.bits() >> 5) & 3) {
        case 0: res = RVC_RS1S ^ RVC_RS2S; // c.xor
        case 1: res = int32_t(RVC_RS1S) >> (RVC_RS2S & 0x1f); // c.sra
        default: require(0);
      }
      WRITE_RVC_RS1S(res);
      break;

    case 1:
      switch ((insn.bits() >> 5) & 3) {
        case 0: res = RVC_RS1S << (RVC_RS2S & 0x1f); // c.sll
        case 1: res = uint32_t(RVC_RS1S) >> (RVC_RS2S & 0x1f); // c.srl
        case 2: res = int32_t(RVC_RS1S) < int32_t(RVC_RS2S); // c.slt
        case 3: res = uint32_t(RVC_RS1S) < uint32_t(RVC_RS2S); // c.sltu
      }
      WRITE_RVC_RS1S(res);
      break;

    case 3:
      switch ((insn.bits() >> 5) & 3) {
        case 0: res = RVC_RS1S << (RVC_RS2S & 0x1f); // c.sllr
        case 1: res = uint32_t(RVC_RS1S) >> (RVC_RS2S & 0x1f); // c.srlr
        case 2: res = int32_t(RVC_RS1S) < int32_t(RVC_RS2S); // c.sltr
        case 3: res = uint32_t(RVC_RS1S) < uint32_t(RVC_RS2S); // c.sltur
      }
      WRITE_RVC_RS2S(res);
      break;

    default: require(0);
  }
} else {
  MMU.store_uint64(RVC_RS1S + insn.rvc_ld_imm(), RVC_RS2S);
}
