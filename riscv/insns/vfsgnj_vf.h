// vfsgnj vd, vs2, vs1
VI_VFP_VF_LOOP
({
  vd = fsgnj8_1(vs2.v, rs1.v, false, false);
},
{
  vd = fsgnj8_2(vs2.v, rs1.v, false, false);
},
{
  vd = fsgnj16(vs2.v, rs1.v, false, false);
},
{
  vd = fsgnj32(vs2.v, rs1.v, false, false);
},
{
  vd = fsgnj64(vs2.v, rs1.v, false, false);
})
