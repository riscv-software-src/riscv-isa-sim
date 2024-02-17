// vfsgnx
VI_VFP_VF_LOOP
({
  vd = fsgnj8_1(vs2.v, rs1.v, false, true);
},
{
  vd = fsgnj8_2(vs2.v, rs1.v, false, true);
},
{
  vd = fsgnj16(vs2.v, rs1.v, false, true);
},
{
  vd = fsgnj32(vs2.v, rs1.v, false, true);
},
{
  vd = fsgnj64(vs2.v, rs1.v, false, true);
})
