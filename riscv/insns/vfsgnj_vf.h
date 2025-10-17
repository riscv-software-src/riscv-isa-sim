// vfsgnj vd, vs2, vs1
VI_VFP_VF_LOOP
({
  vd = P.VU.altfmt ? bfsgnj16(vs2.v, rs1.v, false, false)
                   :  fsgnj16(vs2.v, rs1.v, false, false);
},
{
  vd = fsgnj32(vs2.v, rs1.v, false, false);
},
{
  vd = fsgnj64(vs2.v, rs1.v, false, false);
})
