// vfsgnj
VI_VFP_VV_LOOP
({
  vd = P.VU.altfmt ? bfsgnj16(vs2.v, vs1.v, false, false)
                   :  fsgnj16(vs2.v, vs1.v, false, false);
},
{
  vd = fsgnj32(vs2.v, vs1.v, false, false);
},
{
  vd = fsgnj64(vs2.v, vs1.v, false, false);
})
