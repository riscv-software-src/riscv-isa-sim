// vfsgnx
VI_VFP_VF_LOOP
({
  rs1 = P.VU.altfmt ? READ_FREG_BF(rs1_num) : rs1;
  vd = P.VU.altfmt ? bfsgnj16(vs2.v, rs1.v, false, true)
                   :  fsgnj16(vs2.v, rs1.v, false, true);
},
{
  vd = fsgnj32(vs2.v, rs1.v, false, true);
},
{
  vd = fsgnj64(vs2.v, rs1.v, false, true);
})
