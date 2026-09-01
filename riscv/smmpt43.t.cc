#include "mmu.h"
#include "encoding.h"
#include "cfg.h"
#include "isa_parser.h"
#include "processor.h"
#include "simif.h"
#include "trap.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

static constexpr reg_t root = 0x100000;
static constexpr reg_t l1 = 0x200000;
static constexpr reg_t l0 = 0x300000;
static constexpr reg_t mmio_root = 0x400000;
static constexpr reg_t test_pa = 0x12345000;
static constexpr reg_t test_value = 0x1234abcd5678ef00;

static reg_t mmpt43(reg_t base)
{
  return (reg_t(MMPT_MODE_43) << 60) | (base >> PGSHIFT);
}

static reg_t table_mpte(reg_t base)
{
  return MPTE_V | ((base >> PGSHIFT) << MPTE_PPN_SHIFT);
}

static reg_t leaf_mpte(reg_t xwr = MPTE_XWR_R | MPTE_XWR_W | MPTE_XWR_X)
{
  reg_t mpte = MPTE_V | MPTE_L;
  for (unsigned i = 0; i < 16; ++i)
    mpte |= xwr << (MPTE_XWR_SHIFT + i * 3);
  return mpte;
}

static reg_t leaf_mpte_tuple(unsigned tuple, reg_t xwr)
{
  return (leaf_mpte(0) & ~(MPTE_XWR_MASK << (MPTE_XWR_SHIFT + tuple * 3))) |
         (xwr << (MPTE_XWR_SHIFT + tuple * 3));
}

static reg_t napot_mpte(reg_t xwr = MPTE_XWR_R | MPTE_XWR_W | MPTE_XWR_X, reg_t g = 4)
{
  return MPTE_V | MPTE_L | MPTE_N | (xwr << MPTE_XWR_SHIFT) |
         (g << MPTE_NAPOT_G_SHIFT);
}

static reg_t mpte_addr(reg_t base, reg_t pa, int level)
{
  const reg_t idx = (pa >> (16 + level * 9)) & 0x1ff;
  return base + idx * sizeof(uint64_t);
}

static unsigned tuple_index(reg_t pa, int level)
{
  return level > 0 ? ((pa >> (12 + (level - 1) * 9 + 5)) & 0xf)
                   : ((pa >> 12) & 0xf);
}

class fixture_t {
 public:
  fixture_t()
  {
    pt[mpte_addr(root, test_pa, 2)] = table_mpte(l1);
    pt[mpte_addr(l1, test_pa, 1)] = table_mpte(l0);
    pt[mpte_addr(l0, test_pa, 0)] = leaf_mpte();
  }

  smmpt43_mpte_loader_t loader()
  {
    return [this](reg_t addr, reg_t* mpte) {
      auto it = pt.find(addr);
      if (it == pt.end())
        return false;
      *mpte = it->second;
      return true;
    };
  }

  smmpt_status_t check(reg_t pa, reg_t len, access_type type, reg_t mode = PRV_S)
  {
    return smmpt43_check(mmpt43(root), pa, len, type, mode, loader());
  }

  std::unordered_map<reg_t, reg_t> pt;
};

class fake_sim_t : public simif_t {
 public:
  explicit fake_sim_t(cfg_t& cfg) : cfg(cfg) {}

  char* addr_to_mem(reg_t paddr) override
  {
    auto it = pages.find(paddr & -PGSIZE);
    if (it == pages.end())
      return nullptr;
    return reinterpret_cast<char*>(it->second.data() + (paddr & (PGSIZE - 1)));
  }

  bool mmio_load(reg_t paddr, size_t len, uint8_t* bytes) override
  {
    if (len != sizeof(uint64_t))
      return false;
    auto it = mmio.find(paddr);
    if (it == mmio.end())
      return false;

    target_endian<uint64_t> target = target_endian<uint64_t>::to_le(it->second);
    std::memcpy(bytes, &target, sizeof(target));
    return true;
  }

  bool mmio_store(reg_t, size_t, const uint8_t*) override { return false; }
  void proc_reset(unsigned) override {}
  const cfg_t& get_cfg() const override { return cfg; }
  const std::map<size_t, processor_t*>& get_harts() const override { return harts; }
  const char* get_symbol(uint64_t) override { return nullptr; }

  void write_host64(reg_t paddr, uint64_t value)
  {
    auto& page = pages[paddr & -PGSIZE];
    target_endian<uint64_t> target = target_endian<uint64_t>::to_le(value);
    std::memcpy(page.data() + (paddr & (PGSIZE - 1)), &target, sizeof(target));
  }

  void write_mmio64(reg_t paddr, uint64_t value)
  {
    mmio[paddr] = value;
  }

 private:
  cfg_t& cfg;
  std::map<size_t, processor_t*> harts;
  std::unordered_map<reg_t, std::array<uint8_t, PGSIZE>> pages;
  std::unordered_map<reg_t, uint64_t> mmio;
};

static int failures = 0;

static void expect(const char* name, bool value)
{
  if (!value) {
    std::fprintf(stderr, "FAIL: %s\n", name);
    failures++;
  }
}

static void test_inactive_modes()
{
  fixture_t f;
  expect("off mode", smmpt43_check(0, test_pa, 8, LOAD, PRV_S, f.loader()) == smmpt_status_t::OFF);
  expect("M mode bypass", f.check(test_pa, 8, LOAD, PRV_M) == smmpt_status_t::OFF);
  expect("zero length pass", f.check(test_pa, 0, LOAD) == smmpt_status_t::PASS);
}

static void test_permissions()
{
  fixture_t f;
  expect("read leaf", f.check(test_pa, 8, LOAD) == smmpt_status_t::PASS);
  expect("write leaf", f.check(test_pa, 8, STORE) == smmpt_status_t::PASS);
  expect("execute leaf", f.check(test_pa, 2, FETCH) == smmpt_status_t::PASS);

  f.pt[mpte_addr(l0, test_pa, 0)] = leaf_mpte(MPTE_XWR_R);
  expect("write denied", f.check(test_pa, 8, STORE) == smmpt_status_t::FAULT);
  expect("execute denied", f.check(test_pa, 2, FETCH) == smmpt_status_t::FAULT);

  f.pt[mpte_addr(l0, test_pa, 0)] = leaf_mpte(MPTE_XWR_X | MPTE_XWR_R);
  expect("read execute allowed", f.check(test_pa, 8, LOAD) == smmpt_status_t::PASS);
  expect("read execute store denied", f.check(test_pa, 8, STORE) == smmpt_status_t::FAULT);
}

static void test_range_checks()
{
  fixture_t f;
  expect("high address denied", f.check(reg_t(1) << 43, 1, LOAD) == smmpt_status_t::FAULT);
  expect("wrapped range denied", f.check(~reg_t(0) - 3, 8, LOAD) == smmpt_status_t::FAULT);

  const reg_t pa1 = test_pa + PGSIZE;
  f.pt[mpte_addr(l0, pa1, 0)] = leaf_mpte(MPTE_XWR_R);
  expect("cross-page pass", f.check(test_pa + PGSIZE - 4, 8, LOAD) == smmpt_status_t::PASS);

  f.pt[mpte_addr(l0, pa1, 0)] = leaf_mpte(MPTE_XWR_W | MPTE_XWR_R);
  expect("cross-page fault", f.check(test_pa + PGSIZE - 4, 8, FETCH) == smmpt_status_t::FAULT);

  const reg_t pa16 = (test_pa & ~reg_t(0xffff)) + 0x10000;
  f.pt[mpte_addr(l0, test_pa, 0)] = leaf_mpte_tuple(15, MPTE_XWR_R);
  f.pt[mpte_addr(l0, pa16, 0)] = leaf_mpte_tuple(tuple_index(pa16, 0), MPTE_XWR_X);
  expect("cross-range tuple fault", f.check(pa16 - 4, 8, LOAD) == smmpt_status_t::FAULT);
}

static void test_bad_mptes()
{
  fixture_t f;
  f.pt[mpte_addr(l0, test_pa, 0)] = 0;
  expect("invalid zero mpte", f.check(test_pa, 8, LOAD) == smmpt_status_t::FAULT);

  f = fixture_t();
  f.pt[mpte_addr(l0, test_pa, 0)] = leaf_mpte(MPTE_XWR_R) | _RISCV_ULL(1) << 56;
  expect("reserved leaf bit", f.check(test_pa, 8, LOAD) == smmpt_status_t::FAULT);

  f = fixture_t();
  f.pt[mpte_addr(l0, test_pa, 0)] = leaf_mpte(MPTE_XWR_W);
  expect("reserved write-only xwr", f.check(test_pa, 8, STORE) == smmpt_status_t::FAULT);

  f = fixture_t();
  f.pt[mpte_addr(l0, test_pa, 0)] = leaf_mpte(MPTE_XWR_X | MPTE_XWR_W);
  expect("reserved execute-write xwr", f.check(test_pa, 8, FETCH) == smmpt_status_t::FAULT);

  f = fixture_t();
  f.pt[mpte_addr(root, test_pa, 2)] = table_mpte(l1) | MPTE_N;
  expect("nonleaf with N", f.check(test_pa, 8, LOAD) == smmpt_status_t::FAULT);

  f = fixture_t();
  f.pt[mpte_addr(root, test_pa, 2)] = table_mpte(l1) | _RISCV_ULL(1) << 2;
  expect("nonleaf reserved bit", f.check(test_pa, 8, LOAD) == smmpt_status_t::FAULT);

  f = fixture_t();
  f.pt.erase(mpte_addr(l1, test_pa, 1));
  expect("mpte load failure", f.check(test_pa, 8, LOAD) == smmpt_status_t::FAULT);
}

static void test_leaf_levels()
{
  fixture_t f;
  const unsigned l2_tuple = tuple_index(test_pa, 2);
  f.pt[mpte_addr(root, test_pa, 2)] = leaf_mpte_tuple(l2_tuple, MPTE_XWR_R);
  expect("l2 leaf read", f.check(test_pa, 8, LOAD) == smmpt_status_t::PASS);
  expect("l2 leaf write denied", f.check(test_pa, 8, STORE) == smmpt_status_t::FAULT);

  f = fixture_t();
  const unsigned l1_tuple = tuple_index(test_pa, 1);
  f.pt[mpte_addr(l1, test_pa, 1)] = leaf_mpte_tuple(l1_tuple, MPTE_XWR_X);
  expect("l1 leaf fetch", f.check(test_pa, 2, FETCH) == smmpt_status_t::PASS);
  expect("l1 leaf load denied", f.check(test_pa, 8, LOAD) == smmpt_status_t::FAULT);
}

static void test_napot()
{
  fixture_t f;
  f.pt[mpte_addr(l0, test_pa, 0)] = napot_mpte(MPTE_XWR_R);
  expect("napot read", f.check(test_pa, 8, LOAD) == smmpt_status_t::PASS);
  expect("napot write denied", f.check(test_pa, 8, STORE) == smmpt_status_t::FAULT);

  f.pt[mpte_addr(l0, test_pa, 0)] = napot_mpte(MPTE_XWR_W);
  expect("napot reserved xwr", f.check(test_pa, 8, STORE) == smmpt_status_t::FAULT);

  f.pt[mpte_addr(l0, test_pa, 0)] = napot_mpte(MPTE_XWR_R, 3);
  expect("napot reserved g low", f.check(test_pa, 8, LOAD) == smmpt_status_t::FAULT);

  f.pt[mpte_addr(l0, test_pa, 0)] = napot_mpte(MPTE_XWR_R, 5);
  expect("napot reserved g high", f.check(test_pa, 8, LOAD) == smmpt_status_t::FAULT);

  f.pt[mpte_addr(l0, test_pa, 0)] = napot_mpte(MPTE_XWR_R) | _RISCV_ULL(1) << 16;
  expect("napot reserved bit", f.check(test_pa, 8, LOAD) == smmpt_status_t::FAULT);
}

static void install_host_tables(fake_sim_t& sim, reg_t leaf = leaf_mpte(MPTE_XWR_R))
{
  sim.write_host64(mpte_addr(root, test_pa, 2), table_mpte(l1));
  sim.write_host64(mpte_addr(l1, test_pa, 1), table_mpte(l0));
  sim.write_host64(mpte_addr(l0, test_pa, 0), leaf);
  sim.write_host64(test_pa, test_value);
}

static void install_mmio_root(fake_sim_t& sim)
{
  sim.write_mmio64(mpte_addr(mmio_root, test_pa, 2), table_mpte(l1));
}

static bool load_traps_with_cause(processor_t& proc, reg_t cause)
{
  try {
    (void)proc.get_mmu()->load<uint64_t>(test_pa);
    return false;
  } catch (trap_t& t) {
    return t.cause() == cause;
  }
}

static void test_isa_and_csr()
{
  isa_parser_t by_mode("rv64i_smmpt43", "MSU");
  isa_parser_t by_family("rv64i_smmpt", "MSU");
  expect("smmpt43 isa extension bit", by_mode.extension_enabled(EXT_SMMPT));
  expect("smmpt isa extension bit", by_family.extension_enabled(EXT_SMMPT));

  cfg_t cfg;
  cfg.isa = "rv64imac_smmpt43";
  cfg.priv = "MSU";
  fake_sim_t sim(cfg);
  processor_t proc(cfg.isa, cfg.priv, &cfg, &sim, 0, false, stderr, std::cerr);

  proc.put_csr(CSR_MMPT, (reg_t(2) << 60) | 0x12345);
  expect("unsupported mmpt mode warl zero", proc.get_csr(CSR_MMPT) == 0);

  const reg_t ppn = 0x12345;
  proc.put_csr(CSR_MMPT, (reg_t(MMPT_MODE_43) << 60) | (_RISCV_ULL(0x3f) << 52) | ppn);
  expect("mmpt mode43 with zero sdid", proc.get_csr(CSR_MMPT) == ((reg_t(MMPT_MODE_43) << 60) | ppn));

  proc.put_csr(CSR_MMPT, 0);
  expect("mmpt bare clears ppn", proc.get_csr(CSR_MMPT) == 0);
}

static void test_live_mmu_integration()
{
  cfg_t cfg;
  cfg.isa = "rv64imac_smmpt43";
  cfg.priv = "MSU";
  fake_sim_t sim(cfg);
  processor_t proc(cfg.isa, cfg.priv, &cfg, &sim, 0, false, stderr, std::cerr);
  proc.get_state()->prv = PRV_S;

  install_host_tables(sim);
  proc.put_csr(CSR_MMPT, mmpt43(root));
  expect("live mmu host mpte load pass", proc.get_mmu()->load<uint64_t>(test_pa) == test_value);

  sim.write_host64(mpte_addr(l0, test_pa, 0), leaf_mpte(MPTE_XWR_X));
  proc.get_mmu()->flush_tlb();
  expect("live mmu denied load traps", load_traps_with_cause(proc, CAUSE_LOAD_ACCESS));

  proc.get_state()->prv = PRV_M;
  expect("live mmu m-mode bypass", proc.get_mmu()->load<uint64_t>(test_pa) == test_value);
  proc.get_state()->prv = PRV_S;

  sim.write_host64(mpte_addr(l0, test_pa, 0), leaf_mpte(MPTE_XWR_R));
  install_mmio_root(sim);
  proc.put_csr(CSR_MMPT, mmpt43(mmio_root));
  expect("live mmu mmio mpte load pass", proc.get_mmu()->load<uint64_t>(test_pa) == test_value);

  proc.put_csr(CSR_MMPT, mmpt43(0x700000));
  expect("live mmu missing mpte traps", load_traps_with_cause(proc, CAUSE_LOAD_ACCESS));

  proc.put_csr(CSR_PMPADDR0, (l0 + PGSIZE) >> PMP_SHIFT);
  proc.put_csr(CSR_PMPADDR1, ~reg_t(0));
  proc.put_csr(CSR_PMPCFG0, (PMP_TOR | PMP_L) | ((PMP_R | PMP_W | PMP_X | PMP_NAPOT) << 8));
  proc.put_csr(CSR_MMPT, mmpt43(root));
  expect("live mmu mpte pmp deny traps", load_traps_with_cause(proc, CAUSE_LOAD_ACCESS));
}

int main()
{
  test_inactive_modes();
  test_permissions();
  test_range_checks();
  test_bad_mptes();
  test_leaf_levels();
  test_napot();
  test_isa_and_csr();
  test_live_mmu_integration();
  return failures == 0 ? 0 : 1;
}
