#include <svdpi.h>

#include <cassert>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

/* Temporarily turn all private members of Spike classes into public so we
 * can call sim_t::step() (kept private upstream). */
#pragma push_macro("private")
#undef  private
#define private public
#include "sim.h"
#include "processor.h"
#include "mmu.h"
#pragma pop_macro("private")

#include "cfg.h"           /* cfg_t helper                          */
#include "debug_module.h"  /* debug_module_config_t                 */
#include "encoding.h"      /* CSR_MIP constant                      */

/* --------------------------------------------------------------------- */
/*                          Helper: build sim_t                          */
/* --------------------------------------------------------------------- */
static std::unique_ptr<sim_t>
build_sim(const std::vector<std::string>& argv)
{
    static auto cfg = std::make_unique<cfg_t>(); // default-initialised

    return std::make_unique<sim_t>(          // Call the sim_t constructor
        cfg.get(),                           // halted_at_reset = false
        false,
        std::vector<std::pair<reg_t, abstract_mem_t*>>{},  // no extra mem
        std::vector<device_factory_sargs_t>{},             // no extra IO
        argv,                                // command-line arguments
        debug_module_config_t{}, nullptr,    // default DM
        false, nullptr,                      // no DTB
        false, nullptr,                      // no socket server
        std::nullopt);                       // unlimited trace
}

/* ================================================================= */
/*                         Spike DPI Wrapper                         */
/* ================================================================= */
class SpikeDpiWrapper {
public:
    SpikeDpiWrapper(int argc, const char* const* argv)
    {
        std::vector<std::string> av(argv, argv + argc);
        sim = build_sim(av);
        if (!sim)
            throw std::runtime_error("Spike initialisation failed");
    }

    /* One-instruction stepping (sim_t::step() is normally private). */
    void step(uint64_t n = 1)          { sim->step(n); }
    /* Run until HTIF exit. */
    void run()                         { sim->run();   }
    int  exit_code() const             { return sim->exit_code(); }

    /*  read current PC of core 0 */
    uint64_t pc() const { return sim->get_core(0)->get_state()->pc; }

    /* ---------------- Memory helpers ---------------- */
    int load_u64(uint64_t* dst, uint64_t addr)
    {
        *dst = sim->get_core(0)->get_mmu()->template load<uint64_t>(addr);
        return 0;
    }

    int store_bytes(uint64_t addr, uint64_t data, unsigned sz_bits)
    {
        auto* mmu = sim->get_core(0)->get_mmu();
        if      (sz_bits ==  8) mmu->template store<uint8_t >(addr, data);
        else if (sz_bits == 16) mmu->template store<uint16_t>(addr, data);
        else if (sz_bits == 32) mmu->template store<uint32_t>(addr, data);
        else if (sz_bits == 64) mmu->template store<uint64_t>(addr, data);
        else                    return -1;
        return 0;
    }

    /* ---------------- Interrupt helper -------------- */
    void set_mip(uint64_t mip_val)
    {
        sim->get_core(0)->put_csr(CSR_MIP, mip_val);
    }

    /* ---------------- Dummy translator -------------- */
    uint64_t translate(uint64_t vaddr, uint8_t /*acc*/, uint64_t /*satp*/,
                       uint64_t /*priv*/, uint64_t /*mstatus*/,
                       uint64_t* exc)
    {
        if (exc) *exc = 0;
        return vaddr;  /* identity mapping stub */
    }

private:
    std::unique_ptr<sim_t> sim;
};

/* Single global instance */
static std::unique_ptr<SpikeDpiWrapper> g_spike;

/* ===================================================================== */
/*                          DPI-visible shims                            */
/* ===================================================================== */
extern "C" void spike_setup(long long /*argc*/, const char* argv_flat)
{
    /* Tokenise argv_flat (space separated). */
    std::vector<std::string> toks;
    std::string cur;
    for (const char* p = argv_flat; *p; ++p) {
        if (*p == ' ') { if (!cur.empty()) { toks.push_back(cur); cur.clear(); } }
        else            cur.push_back(*p);
    }
    if (!cur.empty()) toks.push_back(cur);

    std::vector<const char*> c_argv;
    for (auto& s : toks) c_argv.push_back(s.c_str());

    g_spike = std::make_unique<SpikeDpiWrapper>(c_argv.size(), c_argv.data());
}

extern "C" void start_execution()                         { g_spike->run();            }
extern "C" void do_step(unsigned long long n)             { g_spike->step(n);          }
extern "C" int  exit_code()                               { return g_spike->exit_code(); }

extern "C" uint64_t spike_get_pc() { return g_spike ? g_spike->pc() : 0ull; }

extern "C" int  get_memory_data(uint64_t* d,uint64_t a)    { return g_spike->load_u64(d,a); }
extern "C" int  set_memory_data(uint64_t d,uint64_t a,int s){ return g_spike->store_bytes(a,d,s); }
extern "C" void spike_set_external_interrupt(uint64_t m)   { g_spike->set_mip(m);       }

extern "C" uint64_t address_translate(uint64_t v, uint64_t len, int acc,
                                       uint64_t satp, uint64_t prv,
                                       uint64_t ms, uint64_t* exc)
{
    (void)len;
    return g_spike->translate(v, static_cast<uint8_t>(acc), satp, prv, ms, exc);
}

/* ---------- Compatibility stubs (unimplemented) ---------------------- */
extern "C" int  run_and_inject(uint32_t, void*)                 { return -1; }
extern "C" int  spike_run_until_vector_ins(void*)               { return -1; }
extern "C" int  spike_run_until_target_ins(uint64_t, void*)     { return -1; }
extern "C" void get_src_vreg(int, const svOpenArrayHandle)      {}
extern "C" void get_dst_vreg(int, const svOpenArrayHandle)      {}
extern "C" int  get_mem_addr(const svOpenArrayHandle)           { return 0; }
extern "C" int  get_mem_elem(const svOpenArrayHandle)           { return 0; }
extern "C" uint64_t spike_get_mstatus()                         { return 0; }
extern "C" uint64_t l2_address_translate(uint64_t,uint64_t,int,
                                         uint64_t,uint64_t,uint64_t,
                                         uint64_t*,void*)        { return 0; }
extern "C" void read_elf(const char*)                           {}
extern "C" unsigned char get_symbol_addr(const char*,uint64_t*){ return 0; }
extern "C" unsigned char is_vector(uint64_t)                    { return 0; }
