// vmfirst rd, vs2, vm
require(STATE.VU.vsew >= e8 && STATE.VU.vsew <= e64);
require(!STATE.VU.vill);
reg_t vl = STATE.VU.vl;
reg_t sew = STATE.VU.vsew;
reg_t rd_num = insn.rd();
reg_t rs2_num = insn.rs2();
require(STATE.VU.vstart == 0);
reg_t pos = -1;
for (reg_t i=STATE.VU.vstart; i<vl; ++i){
  const int mlen = STATE.VU.vmlen;
  const int midx = (mlen * i) / 32;
  const int mpos = (mlen * i) % 32;

  bool vs2_lsb = ((STATE.VU.elt<uint32_t>(rs2_num, midx ) >> mpos) & 0x1) == 1;
  if (insn.v_vm() == 1) {
    pos = i;
  } else {
    bool do_mask = (STATE.VU.elt<uint32_t>(0, midx) >> mpos) & 0x1;
    pos = i;
  }
}
STATE.VU.vstart = 0;
WRITE_RD(pos);
