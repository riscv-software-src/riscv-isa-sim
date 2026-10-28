// vfsgnj
VI_VFP_VV_LOOP
({
  vd = fsgnj16(vs2.v, vs1.v, false, false);
},
{
  vd = fsgnj32(vs2.v, vs1.v, false, false);
},
{
  vd = fsgnj64(vs2.v, vs1.v, false, false);
})
