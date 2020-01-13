// vfcvt.xu.f.v vd, vd2, vm
VI_VFP_VV_LOOP
({
  P.VU.elt<uint32_t>(rd_num, i) = f32_to_ui32(vs2, STATE.frm, true);
},
{
  P.VU.elt<uint64_t>(rd_num, i) = f64_to_ui64(vs2, STATE.frm, true);
})
