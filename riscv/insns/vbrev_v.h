// vbrev.v vd, vs2

#include "zvk_ext_macros.h"

require_zvbb;

VI_V_ULOOP
({
  reg_t x = vs2;

  // Reverse bits in bytes (vbrev8)
  x = ((x & 0x5555555555555555llu) <<  1) | ((x & 0xAAAAAAAAAAAAAAAAllu) >>  1);
  x = ((x & 0x3333333333333333llu) <<  2) | ((x & 0xCCCCCCCCCCCCCCCCllu) >>  2);
  x = ((x & 0x0F0F0F0F0F0F0F0Fllu) <<  4) | ((x & 0xF0F0F0F0F0F0F0F0llu) >>  4);
  // Re-order bytes (vrev8)
  if (P.VU.vsew > 8)
    x = ((x & 0x00FF00FF00FF00FFllu) <<  8) | ((x & 0xFF00FF00FF00FF00llu) >>  8);
  if (P.VU.vsew > 16)
    x = ((x & 0x0000FFFF0000FFFFllu) << 16) | ((x & 0xFFFF0000FFFF0000llu) >> 16);
  if (P.VU.vsew > 32)
    x = ((x & 0x00000000FFFFFFFFllu) << 32) | ((x & 0xFFFFFFFF00000000llu) >> 32);

  vd = x;
})
