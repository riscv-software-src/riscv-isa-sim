#include "eviction_policies.h"
#include <stdio.h>

uint32_t lfsr_t::next(size_t idx)
{
    reg = (reg>>1)^(-(reg&1) & 0xd0000001);
    return reg%ways;
}

void lfsr_t::update(uint64_t addr, uint32_t way, const size_t idx_shift) {}

void lfsr_t::insert(size_t idx, size_t way) {}

uint32_t lru_t::lowest_recency_way(size_t idx)
{
    size_t argmin = 0;
    uint32_t min_value = 0xffffffff; // Abitrarily big
    for (size_t i = 0; i < ways; i++) {
      if (min_value > this->metadata[idx*ways+i]) {
        min_value = this->metadata[idx*ways+i];
        argmin = i;
      }
    }
    return argmin;
}

// For set denoted by 'idx', give highest recency to 'way' and
// decrement all elements accessed more recently than 'recency'.
void lru_t::promote(size_t idx, size_t way, uint32_t recency)
{
  this->metadata[idx*ways+way] = ways;
  for (size_t i = 0; i < ways; i++)
    if (this->metadata[idx*ways+i] > recency)
      this->metadata[idx*ways+i]--;
}

// Instruments the 'promote' method to set cachline located in
// [idx][way] as most recently accessed and decrement all other
// cache-lines access recency (all recencies bigger than 0 but
// for the one already at 0).
void lru_t::set_as_mru(size_t idx, size_t way)
{
  this->promote(idx, way, 0);
}

uint32_t lru_t::next(size_t idx)
{
  return this->lowest_recency_way(idx);
}

void lru_t::update(uint64_t addr, uint32_t way, const size_t idx_shift)
{
  size_t idx = (addr >> idx_shift) & (sets-1);
  uint32_t recency = this->metadata[idx*ways+way];
  this->promote(idx, way, recency);
};

void lru_t::insert(size_t idx, size_t way)
{
  this->set_as_mru(idx, way);
}

void fifo_t::update(uint64_t addr, uint32_t way, const size_t idx_shift) {}


void fifo_t::insert(size_t idx, size_t way)
{
  this->set_as_mru(idx, way);
}

void lip_t::update(uint64_t addr, uint32_t way, const size_t idx_shift)
{
  size_t idx = (addr >> idx_shift) & (sets-1);
  if (way == lowest_recency_way(idx))
    this->set_as_mru(idx, way);
}

void lip_t::insert(size_t idx, size_t way)
{
  // Inserted at LRU position
  this->metadata[idx*ways+way] = 0;
}

void bip_t::insert(size_t idx, size_t way)
{
  if (!(lfsr->next(idx)%threshold)) {
    // Inserted at LRU position
    this->metadata[idx*ways+way] = 0;
  }
  else {
    this->set_as_mru(idx, way);
  }
}
