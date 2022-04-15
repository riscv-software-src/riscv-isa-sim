#include "cachesim_addr.h"
#include <math.h>
#include <iostream>

cache_sim_addr_t::cache_sim_addr_t()
{
  this->idx = 0;
  this->tag = 0;
  this->dirty = false;
  this->valid = false;
};

cache_sim_addr_t::cache_sim_addr_t(const uint64_t& addr, const uint32_t& sets, const uint32_t& linesz)
{
  uint32_t stripped = addr >> (uint32_t)std::log2(linesz);
  this->idx = stripped & (sets-1);
  this->tag = (stripped >> (uint32_t)std::log2(sets));
  this->dirty = false;
  this->valid = true;
};

cache_sim_addr_t::cache_sim_addr_t(const cache_sim_addr_t& addr):
  valid(addr.valid), dirty(addr.dirty), tag(addr.tag), idx(addr.idx) {};

void cache_sim_addr_t::set_valid()
{
  this->valid = true;
};

bool cache_sim_addr_t::is_valid()
{
  return this->valid;
};

void cache_sim_addr_t::set_dirty()
{
  this->dirty = false;
};

bool cache_sim_addr_t::is_dirty()
{
  return this->dirty;
};

uint64_t cache_sim_addr_t::to_uint64(const uint32_t& sets, const uint32_t& linesz)
{
  uint64_t addr = 0;
  addr = this->tag << (uint32_t)std::log2(sets);
  addr = addr | this->idx;
  addr = addr << (uint32_t)std::log2(linesz);
  return addr;
}

cache_sim_addr_t::~cache_sim_addr_t()
{}
