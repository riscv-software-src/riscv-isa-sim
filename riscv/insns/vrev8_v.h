// vrev8.v vd, vs2, vm

#include "zvk_ext_macros.h"

require_zvbb;

VI_V_ULOOP
({
  vd = vs2;
  if (P.VU.vsew > 8)
    vd = ((vd & 0x00FF00FF00FF00FFllu) <<  8) | ((vd & 0xFF00FF00FF00FF00llu) >>  8);
  if (P.VU.vsew > 16)
    vd = ((vd & 0x0000FFFF0000FFFFllu) << 16) | ((vd & 0xFFFF0000FFFF0000llu) >> 16);
  if (P.VU.vsew > 32)
    vd = ((vd & 0x00000000FFFFFFFFllu) << 32) | ((vd & 0xFFFFFFFF00000000llu) >> 32);
})
