#include "eviction_policies.h"
#include <iostream>
#include <algorithm>

uint32_t lfsr_t::next(const size_t& idx)
{
    reg = (reg>>1)^(-(reg&1) & 0xd0000001);
    return reg%ways;
}

void lfsr_t::update(const cache_sim_addr_t& addr, const uint32_t& way) {}

void lfsr_t::insert(const size_t& idx, const size_t& way) {}

uint32_t lru_t::lowest_recency_way(const size_t& idx)
{
    auto begin = this->metadata[idx].begin();
    auto end = this->metadata[idx].end();
    return std::distance(begin, std::min_element(begin, end));
}

// For set denoted by 'idx', give highest recency to 'way' and
// decrement all elements accessed more recently than 'recency'.
void lru_t::promote(const size_t& idx, const size_t& way, const uint32_t& recency)
{
  this->metadata[idx][way] = ways;
  for (auto it = this->metadata[idx].begin(); it != this->metadata[idx].end(); it++)
    if (*it > recency)
      (*it)--;
}

// Instruments the 'promote' method to set cachline located in
// [idx][way] as most recently accessed and decrement all other
// cache-lines access recency (all recencies bigger than 0 but
// for the one already at 0).
void lru_t::set_as_mru(const size_t& idx, const size_t& way)
{
  this->metadata[idx][way] = ways;
  for (auto it = this->metadata[idx].begin(); it != this->metadata[idx].end(); it++)
    if (*it > 0)
      (*it)--;
}

uint32_t lru_t::next(const size_t& idx)
{
  return this->lowest_recency_way(idx);
}

void lru_t::update(const cache_sim_addr_t& addr, const uint32_t& way)
{
  uint32_t recency = this->metadata[addr.idx][way];
  this->promote(addr.idx, way, recency);
};

void lru_t::insert(const size_t& idx, const size_t& way)
{
  this->set_as_mru(idx, way);
}

void fifo_t::update(const cache_sim_addr_t& addr, const uint32_t& way)
{}

void lip_t::update(const cache_sim_addr_t& addr, const uint32_t& way)
{
  if (way == lowest_recency_way(addr.idx))
    this->set_as_mru(addr.idx, way);
}

void lip_t::insert(const size_t& idx, const size_t& way)
{
  // Inserted at LRU position
  this->metadata[idx][way] = 0;
}

void bip_t::insert(const size_t& idx, const size_t& way)
{
  if (!(lfsr->next(idx)%threshold)) {
    // Inserted at LRU position
    this->metadata[idx][way] = 0;
  }
  else {
    this->set_as_mru(idx, way);
  }
}
