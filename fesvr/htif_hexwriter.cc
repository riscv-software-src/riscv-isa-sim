// See LICENSE for license details.

#include <iostream>
#include <assert.h>
#include "htif_hexwriter.h"

htif_hexwriter_t::htif_hexwriter_t(size_t b, size_t w, size_t d)
  : base(b), width(w), depth(d)
{
}

void htif_hexwriter_t::read_chunk(addr_t taddr, size_t len, void* vdst)
{
  taddr -= base;

  assert(len % chunk_align() == 0);
  assert(taddr < width*depth);
  assert(taddr+len <= width*depth);

  uint8_t* dst = (uint8_t*)vdst;
  while(len)
  {
    if(mem[taddr/width].size() == 0)
      mem[taddr/width].resize(width,0);

    for(size_t j = 0; j < width; j++)
      dst[j] = mem[taddr/width][j];

    len -= width;
    taddr += width;
    dst += width;
  }
}

void htif_hexwriter_t::write_chunk(addr_t taddr, size_t len, const void* vsrc)
{
  taddr -= base;

  assert(len % chunk_align() == 0);
  assert(taddr < width*depth);
  assert(taddr+len <= width*depth);

  const uint8_t* src = (const uint8_t*)vsrc;
  while(len)
  {
    if(mem[taddr/width].size() == 0)
      mem[taddr/width].resize(width,0);

    for(size_t j = 0; j < width; j++)
      mem[taddr/width][j] = src[j];

    len -= width;
    taddr += width;
  }
}

std::ostream& operator<< (std::ostream& o, const htif_hexwriter_t& h)
{
  std::ios_base::fmtflags flags = o.setf(std::ios::hex,std::ios::basefield);

  for(size_t addr = 0; addr < h.depth; addr++)
  {
    std::map<addr_t,std::vector<char> >::const_iterator i = h.mem.find(addr);
    if(i == h.mem.end())
      for(size_t j = 0; j < h.width; j++)
        o << "00";
    else
      for(size_t j = 0; j < h.width; j++)
        o << ((i->second[h.width-j-1] >> 4) & 0xF) << (i->second[h.width-j-1] & 0xF);
    o << std::endl;
  }

  o.setf(flags);

  return o;
}
