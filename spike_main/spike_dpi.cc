#include <svdpi.h>

#include <cassert>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "decode_macros.h"
#include "disasm.h"

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

/* User-selected ISA string (from --isa option) */
static std::string g_isa_override;
static std::unique_ptr<cfg_t> g_cfg;

/* Simple memory allocator copied from spike_main */
static std::vector<std::pair<reg_t, abstract_mem_t*>>
make_mems(const std::vector<mem_cfg_t>& layout)
{
    std::vector<std::pair<reg_t, abstract_mem_t*>> mems;
    mems.reserve(layout.size());
    for (const auto& cfg : layout)
        mems.emplace_back(cfg.get_base(), new mem_t(cfg.get_size()));
    return mems;
}

/* --------------------------------------------------------------------- */
/*                          Helper: build sim_t                          */
/* --------------------------------------------------------------------- */
static std::unique_ptr<sim_t>
build_sim(const std::vector<std::string>& argv,
          std::vector<std::pair<reg_t, abstract_mem_t*>> mems)
{
    if (!g_cfg)
        g_cfg = std::make_unique<cfg_t>();

    if (!g_isa_override.empty())
        g_cfg->isa = g_isa_override.c_str();

    return std::make_unique<sim_t>(          // Call the sim_t constructor
        g_cfg.get(),                         // halted_at_reset = false
        false,
        std::move(mems),                    // main memory
        std::vector<device_factory_sargs_t>{},             // no extra IO
        argv,                                // command-line arguments
        debug_module_config_t{}, nullptr,    // default DM
        true, nullptr,                       // no DTB
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
        if (!g_cfg)
            g_cfg = std::make_unique<cfg_t>();
        mems = make_mems(g_cfg->mem_layout);
        sim = build_sim(av, mems);
        if (!sim)
            throw std::runtime_error("Spike initialisation failed");
    }

    ~SpikeDpiWrapper()
    {
        for (auto& m : mems)
            delete m.second;
    }

    /* One-instruction stepping (sim_t::step() is normally private). */
    void step(uint64_t n = 1)          
    {
        if (!started) {
            sim->start();
            started = true;
        }

        bool prev_debug = sim->debug;
        sim->set_debug(true);
        //sim->set_procs_debug(true);
        sim->step(n);
        sim->set_debug(prev_debug);
        //sim->set_procs_debug(prev_debug);
    }
    /* Run until HTIF exit. */
    void run()                         { sim->run();   }
    int  exit_code() const             { return sim->exit_code(); }

    /*  read current PC of core 0 */

    /*  read PC of an arbitrary hart */
    uint64_t pc(unsigned hart) const
    {
        if (hart >= sim->get_cfg().nprocs())
            return 0;
        return sim->get_core(hart)->get_state()->pc;
    }

    /* Dump integer registers of a hart */
    void dump_regs(unsigned hart) const
    {
        if (hart >= sim->get_cfg().nprocs())
            return;
        auto* core = sim->get_core(hart);
        int max_xlen = core->get_isa().get_max_xlen();
        std::ostringstream oss;
        oss << "HART " << hart << " RegisterFile:" << std::hex << std::setfill('0');
        for (int r = 0; r < NXPR; ++r) {
            if (r % 4 == 0) oss << "\n";
            oss << std::setw(4) << xpr_name[r] << ": 0x"
                << std::setw(max_xlen/4) << zext(core->get_state()->XPR[r], max_xlen)
                << ' ';
        }
        std::cout << oss.str() << std::dec << std::endl;
    }

    /* Dump CSR registers of a hart */
    void dump_csrs(unsigned hart) const
    {
        if (hart >= sim->get_cfg().nprocs())
            return;
        auto* core = sim->get_core(hart);
        std::ostringstream oss;
        oss << "HART " << hart << " CSRs:";
        for (const auto& it : core->get_state()->csrmap) {
            oss << "\n" << csr_name(it.first) << "(0x" << std::hex << it.first
                << ") : 0x" << it.second->read();
        }
        std::cout << oss.str() << std::dec << std::endl;
    }

    /* Return XPR register value */
    uint64_t get_reg(unsigned hart, unsigned reg) const
    {
        if (hart >= sim->get_cfg().nprocs() || reg >= NXPR)
            return 0;
        return sim->get_core(hart)->get_state()->XPR[reg];
    }

    /* Return floating-point register as 64-bit value (low bits) */
    uint64_t get_freg(unsigned hart, unsigned reg) const
    {
        if (hart >= sim->get_cfg().nprocs() || reg >= NFPR)
            return 0;
        return sim->get_core(hart)->get_state()->FPR[reg].v[0];
    }

    /* Copy vector register contents into buffer */
    void get_vreg(unsigned hart, unsigned reg, void* dest) const
    {
        if (hart >= sim->get_cfg().nprocs() || reg >= NVPR)
            return;
        auto* core = sim->get_core(hart);
        size_t len = core->VU.vlenb;
        std::memcpy(dest,
                    (uint8_t*)core->VU.reg_file + reg * len,
                    len);
    }

    /* Lookup CSR by name and return value */
    uint64_t get_csr(unsigned hart, const char* name) const
    {
        if (hart >= sim->get_cfg().nprocs())
            return 0;
        auto* core = sim->get_core(hart);
        for (int i = 0; i < NCSR; ++i) {
            const char* n = csr_name(i);
            if (n && std::strcmp(n, name) == 0)
                return core->get_csr(i);
        }
        return 0;
    }

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
    std::vector<std::pair<reg_t, abstract_mem_t*>> mems;
    bool started = false;
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
        if (*p == ' ') {
            if (!cur.empty()) {
                toks.push_back(cur);
                cur.clear();
            }
        } else {
            cur.push_back(*p);
        }
    }
    if (!cur.empty())
        toks.push_back(cur);

    std::vector<std::string> args;
    /* Build argv for Spike.  argv[0] should always be program name so
     * that htif option parsing works.  Strip --isa here and remember it. */
    for (const auto& t : toks) {
        if (t.rfind("--isa=", 0) == 0)
            g_isa_override = t.substr(6);
        else
            args.push_back(t);
    }

    std::vector<const char*> c_argv;
    for (auto& s : args)
        c_argv.push_back(s.c_str());

    g_spike = std::make_unique<SpikeDpiWrapper>(c_argv.size(), c_argv.data());
}

extern "C" void start_execution()                         { g_spike->run();            }
extern "C" void do_step(unsigned long long n)             { g_spike->step(n);          }
extern "C" int  exit_code()                               { return g_spike->exit_code(); }

extern "C" uint64_t spike_get_pc(unsigned hart)
{
    return g_spike ? g_spike->pc(hart) : 0ull;
}

extern "C" void spike_dump_registers(unsigned hart)
{
    if (g_spike)
        g_spike->dump_regs(hart);
}

extern "C" void spike_dump_csrs(unsigned hart)
{
    if (g_spike)
        g_spike->dump_csrs(hart);
}


extern "C" uint64_t spike_get_reg(unsigned hart, unsigned reg)
{
    return g_spike ? g_spike->get_reg(hart, reg) : 0ull;
}

extern "C" uint64_t spike_get_freg(unsigned hart, unsigned reg)
{
    return g_spike ? g_spike->get_freg(hart, reg) : 0ull;
}

extern "C" void spike_get_vreg(unsigned hart, unsigned reg, const svOpenArrayHandle dest)
{
//    if (!g_spike || !dest)
//        return;
//    void* ptr = svGetArrayPtr(dest);
//    g_spike->get_vreg(hart, reg, ptr);
}

extern "C" uint64_t spike_get_csr(unsigned hart, const char* name)
{
    return g_spike ? g_spike->get_csr(hart, name) : 0ull;
}


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





// ------------------------------------------------------------------------------------------------------
// Minimal Spike Launch
#include <fesvr/option_parser.h>

static std::vector<std::pair<reg_t, abstract_mem_t*>> make_mems2(const std::vector<mem_cfg_t> &layout)
{
  std::vector<std::pair<reg_t, abstract_mem_t*>> mems;
  mems.reserve(layout.size());
  for (const auto &cfg : layout) {
    mems.push_back(std::make_pair(cfg.get_base(), new mem_t(cfg.get_size())));
  }
  return mems;
}

int spike(int argc, char** argv)
{
  bool debug = false;
  cfg_t cfg;
  option_parser_t parser;
  parser.option('d', 0, 0, [&](const char UNUSED *s){debug = true;});
  parser.option(0, "isa", 1, [&](const char* s){cfg.isa = s;});
  auto argv1 = parser.parse(argv);
  std::vector<std::string> htif_args(argv1, (const char*const*)argv + argc);
  std::vector<std::pair<reg_t, abstract_mem_t*>> mems =
      make_mems2(cfg.mem_layout);

  sim_t s(
      &cfg,                         // halted_at_reset = false
      false,
      std::move(mems),
      std::vector<device_factory_sargs_t>{},
      htif_args,
      debug_module_config_t{}, nullptr, 
      true, nullptr,
      false, nullptr,
      std::nullopt);

  s.set_debug(debug);
  auto return_code = s.run();

  return return_code;
}
// ------------------------------------------------------------------------------------------------------