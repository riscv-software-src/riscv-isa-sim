#include <cstdio>
#include <cstdlib>
#include <memory>

#include <riscv/sim.h>
#include <riscv/processor.h>
#include <riscv/debug_defines.h>
#include <riscv/debug_rom_defines.h>

static void check(bool condition, const char *what)
{
  if (!condition) {
    fprintf(stderr, "FAIL: %s\n", what);
    exit(EXIT_FAILURE);
  }
}

class fixture_t
{
public:
  cfg_t cfg;
  mem_t memory{0x4000};
  std::unique_ptr<sim_t> sim;

  fixture_t(const char *isa, bool autoexec = true, bool auth = false)
  {
    cfg.isa = isa;
    cfg.mem_layout = {mem_cfg_t(DRAM_BASE, memory.size())};
    debug_module_config_t config;
    config.max_sba_data_width = 64;
    config.support_abstractauto = autoexec;
    config.require_authentication = auth;
    sim = std::make_unique<sim_t>(&cfg, false,
        std::vector<std::pair<reg_t, abstract_mem_t *>>{{DRAM_BASE, &memory}},
        std::vector<device_factory_sargs_t>{}, false,
        std::vector<std::string>{"none"}, config, nullptr, true, nullptr,
        false, nullptr, std::nullopt);
    write(DM_DMCONTROL, DM_DMCONTROL_DMACTIVE);
  }

  debug_module_t& dm() { return *sim->debug_module; }

  uint32_t read(unsigned address)
  {
    uint32_t value;
    check(dm().dmi_read(address, &value), "DMI read succeeds");
    return value;
  }

  void write(unsigned address, uint32_t value)
  {
    check(dm().dmi_write(address, value), "DMI write succeeds");
  }

  void idle()
  {
    for (unsigned i = 0; i < 25; i++)
      dm().run_test_idle();
  }

  void halt()
  {
    write(DM_DMCONTROL, DM_DMCONTROL_DMACTIVE | DM_DMCONTROL_HALTREQ);
    for (unsigned i = 0; i < 100 && !(read(DM_DMSTATUS) & DM_DMSTATUS_ALLHALTED); i++)
      sim->get_core(size_t(0))->step(10);
    check(read(DM_DMSTATUS) & DM_DMSTATUS_ALLHALTED, "hart halted");
  }

  unsigned finish_command()
  {
    for (unsigned i = 0; i < 100 && (read(DM_ABSTRACTCS) & DM_ABSTRACTCS_BUSY); i++) {
      sim->get_core(size_t(0))->step(10);
      dm().run_test_idle();
    }
    auto status = read(DM_ABSTRACTCS);
    check(!(status & DM_ABSTRACTCS_BUSY), "abstract command completed");
    return get_field(status, DM_ABSTRACTCS_CMDERR);
  }
};

static void test_sba_reset(const char *isa, bool read_access)
{
  fixture_t f(isa);
  const uint8_t sentinel = 0x5a;
  check(f.memory.store(0, 1, &sentinel), "initialize memory");
  if (read_access) {
    f.write(DM_SBCS, DM_SBCS_SBREADONADDR | (2 << DM_SBCS_SBACCESS_OFFSET));
    f.write(DM_SBADDRESS0, DRAM_BASE);
  } else {
    f.write(DM_SBADDRESS0, DRAM_BASE);
    f.write(DM_SBDATA0, 0xa5);
  }
  check(f.read(DM_SBCS) & DM_SBCS_SBBUSY, "SBA request pending");
  f.write(DM_DMCONTROL, 0);
  // Exercise both time spent inactive and immediate reactivation.
  if (!read_access)
    f.idle();
  f.write(DM_DMCONTROL, DM_DMCONTROL_DMACTIVE);
  check(!(f.read(DM_SBCS) & DM_SBCS_SBBUSY), "reset cancels SBA request");
  f.idle();
  check(!(f.read(DM_SBCS) & (DM_SBCS_SBERROR | DM_SBCS_SBBUSYERROR)),
        "cancelled SBA request causes no late error");
  check(f.read(DM_SBADDRESS0) == 0 && f.read(DM_SBDATA0) == 0,
        "cancelled SBA request does not update registers");
  uint8_t value;
  check(f.memory.load(0, 1, &value) && value == sentinel,
        "cancelled SBA write does not reach memory");
  check(get_field(f.read(DM_SBCS), DM_SBCS_SBACCESS) == 2, "sbaccess resets to 32 bits");
  // A new access after reset must still work.
  f.write(DM_SBADDRESS0, DRAM_BASE);
  f.write(DM_SBDATA0, 0x12345678);
  f.idle();
  f.write(DM_SBCS, DM_SBCS_SBREADONADDR | (2 << DM_SBCS_SBACCESS_OFFSET));
  f.write(DM_SBADDRESS0, DRAM_BASE);
  f.idle();
  check(f.read(DM_SBDATA0) == 0x12345678, "SBA recovers after reset");
}

static void test_inactive(const char *isa)
{
  fixture_t f(isa);
  f.halt();
  const auto havereset = f.read(DM_DMSTATUS) & DM_DMSTATUS_ALLHAVERESET;
  const auto data_address = get_field(f.read(DM_HARTINFO), DM_HARTINFO_DATAADDR);
  f.write(DM_DATA0, 0x12345678);
  f.write(DM_PROGBUF0, 0x12345678);
  f.write(DM_SBADDRESS0, DRAM_BASE);
  f.write(DM_HAWINDOW, 1);
  f.write(DM_HAWINDOWSEL, 1);
  f.write(DM_DMCS2, (7 << DM_DMCS2_GROUP_OFFSET) | DM_DMCS2_HGWRITE);
  f.write(DM_COMMAND, 0xff000000); // Remember an unsupported command.
  check(get_field(f.read(DM_ABSTRACTCS), DM_ABSTRACTCS_CMDERR) == CMDERR_NOTSUP,
        "unsupported command recorded");
  f.write(DM_DMCONTROL, 0);
  check(f.read(DM_DMCONTROL) == 0, "DM deactivated");
  check(f.sim->get_core(size_t(0))->halt_request == processor_t::HR_NONE,
        "deactivation clears halt request immediately");
  for (unsigned address : {DM_DATA0, DM_PROGBUF0, DM_SBADDRESS0, DM_SBDATA0,
                           DM_HAWINDOW, DM_HAWINDOWSEL, DM_ABSTRACTAUTO, DM_COMMAND,
                           DM_DMCS2, DM_SBCS, DM_AUTHDATA}) {
    f.write(address, 0xffffffff);
    check(f.read(address) == 0, "inactive register access has no effect");
  }
  // A hart can still be finishing an old command while the DM is inactive.
  const uint8_t late_data[] = {1, 2, 3, 4};
  check(f.dm().store(data_address, sizeof(late_data), late_data), "late hart write accepted");
  check(f.read(DM_DATA0) == 0, "late hart write is hidden while inactive");
  f.idle();
  f.write(DM_DMCONTROL, DM_DMCONTROL_DMACTIVE);
  for (unsigned address : {DM_DATA0, DM_PROGBUF0, DM_SBADDRESS0, DM_SBDATA0,
                           DM_HAWINDOW, DM_HAWINDOWSEL, DM_ABSTRACTAUTO, DM_DMCS2})
    check(f.read(address) == 0, "DM register remains reset after activation");
  check(f.read(DM_DMSTATUS) & DM_DMSTATUS_ALLHALTED, "reset preserves halted hart");
  check((f.read(DM_DMSTATUS) & DM_DMSTATUS_ALLHAVERESET) == havereset,
        "reset preserves hart reset history");
  // Autoexec must execute the reset command, not the old unsupported command.
  f.write(DM_ABSTRACTAUTO, 1);
  f.read(DM_DATA0);
  check(f.finish_command() == CMDERR_NONE, "reset clears remembered command");
  f.write(DM_ABSTRACTAUTO, 0);
  // With two progbuf words, branch past both words to the
  // implicit ebreak. This verifies that clearing progbuf preserved it.
  f.write(DM_PROGBUF0, 0x0080006f); // jal zero, +8
  f.write(DM_COMMAND, AC_ACCESS_REGISTER_POSTEXEC);
  check(f.finish_command() == CMDERR_NONE, "reset preserves implicit ebreak");
}

static void test_command_reset(const char *isa)
{
  fixture_t f(isa);
  f.halt();
  f.write(DM_COMMAND, 0);
  check(f.read(DM_ABSTRACTCS) & DM_ABSTRACTCS_BUSY, "abstract command pending");
  f.write(DM_DMCONTROL, 0);
  uint8_t flags;
  check(f.dm().load(DEBUG_ROM_FLAGS, 1, &flags) && flags == 0,
        "deactivation cancels debug ROM request flags");
  f.write(DM_DMCONTROL, DM_DMCONTROL_DMACTIVE);
  check(!(f.read(DM_ABSTRACTCS) & (DM_ABSTRACTCS_BUSY | DM_ABSTRACTCS_CMDERR)),
        "reset clears pending abstract command state");
  f.write(DM_COMMAND, 0);
  check(f.finish_command() == CMDERR_NONE, "abstract command recovers after reset");
}

static void test_autoexec_busy(const char *isa, bool supported)
{
  fixture_t f(isa, supported);
  f.halt();
  f.write(DM_COMMAND, 0);
  check(f.read(DM_ABSTRACTCS) & DM_ABSTRACTCS_BUSY, "abstract command busy");
  f.write(DM_ABSTRACTAUTO, 0xffffffff);
  check(f.finish_command() == (supported ? CMDERR_BUSY : CMDERR_NONE),
        "only implemented abstractauto reports busy access");
  check(f.read(DM_ABSTRACTAUTO) == 0, "busy write does not change abstractauto");
}

static void test_auth_reset(const char *isa)
{
  fixture_t f(isa, true, true);
  f.write(DM_AUTHDATA, f.read(DM_AUTHDATA) + 1);
  check(f.read(DM_DMSTATUS) & DM_DMSTATUS_AUTHENTICATED, "authentication succeeds");
  f.write(DM_DATA0, 0x12345678);
  f.write(DM_DMCONTROL, 0);
  f.write(DM_AUTHDATA, f.read(DM_AUTHDATA) + 1);
  f.write(DM_DMCONTROL, DM_DMCONTROL_DMACTIVE);
  check(!(f.read(DM_DMSTATUS) & DM_DMSTATUS_AUTHENTICATED), "reset requires reauthentication");
  f.write(DM_DATA0, 0xffffffff);
  f.write(DM_AUTHDATA, f.read(DM_AUTHDATA) + 1);
  check(f.read(DM_DMSTATUS) & DM_DMSTATUS_AUTHENTICATED, "reauthentication succeeds");
  check(f.read(DM_DATA0) == 0, "reset and unauthenticated writes leave data clear");
}

int main()
{
  for (const char *isa : {"RV32IMAFDC", "RV64IMAFDC"}) {
    printf("%s debug module reset and busy accesses\n", isa);
    test_sba_reset(isa, false);
    test_sba_reset(isa, true);
    test_inactive(isa);
    test_command_reset(isa);
    test_autoexec_busy(isa, false);
    test_autoexec_busy(isa, true);
    test_auth_reset(isa);
  }
  printf("Executed successfully\n");
}
