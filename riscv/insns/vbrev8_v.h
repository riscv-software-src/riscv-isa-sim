// vbrev8.v vd, vs2, vm

#include "zvk_ext_macros.h"

require_zvbb;

VI_V_ULOOP
({
  vd = vs2;
  vd = ((vd & 0x5555555555555555llu) <<  1) | ((vd & 0xAAAAAAAAAAAAAAAAllu) >>  1);
  vd = ((vd & 0x3333333333333333llu) <<  2) | ((vd & 0xCCCCCCCCCCCCCCCCllu) >>  2);
  vd = ((vd & 0x0F0F0F0F0F0F0F0Fllu) <<  4) | ((vd & 0xF0F0F0F0F0F0F0F0llu) >>  4);
})
