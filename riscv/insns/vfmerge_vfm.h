// vfmerge_vf vd, vs2, vs1, vm
require_zvfbfa

VI_VF_MERGE_LOOP({
//   if (P.VU.vsew == e16 && P.VU.altfmt) {
//   rs1 = ((P.VU.altfmt && P.VU.vsew == e16) ? READ_FREG(rs1_num) : rs1);
//   rs1 = P.VU.altfmt ? READ_FREG_BF(rs1_num) : rs1;
//   rs1 = bf16(READ_FREG(rs1_num));
//   }
  vd = use_first ? rs1 : vs2;
})
