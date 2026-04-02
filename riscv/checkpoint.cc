#include "checkpoint.h"
#include "byteorder.h"
#include "dts.h"
#include "libfdt.h"
#include "processor.h"
#include "sim.h"

#include <fcntl.h>
#include <iomanip>
#include <list>
#include <memory>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>

#define VSEW8 (0x0)
#define VLMUL8 (0x3)

checkpoint_t::checkpoint_t(const checkpoint_config_t &config,
                           simif_t *sim, bus_t *bus, clint_t *clint,
                           std::string dtb, reg_t mainram_base,
                           bool need_trampoline)
    : config(config), sim(sim), bus(bus), clint(clint),
      proc(sim->get_harts().at(0)), dtb(std::move(dtb)),
      mainram_base(mainram_base), need_trampoline(need_trampoline) {}

checkpoint_t::~checkpoint_t() = default;

void checkpoint_t::load() {
  if (!config.snapshot_load_name) return;
  load_cpu();
  load_ram();
}

void checkpoint_t::load_cpu() {
  if (!config.snapshot_load_name) return;
  cpu_deserialize(config.snapshot_load_name);
}

void checkpoint_t::load_ram() {
  if (!config.snapshot_load_name) return;
  ram_deserialize(config.snapshot_load_name);
}

void checkpoint_t::save() {
  if (!config.snapshot_save_name) return;
  cpu_serialize(config.snapshot_save_name);
  ram_serialize(config.snapshot_save_name);
}

// ---------------------------------------------------------------------------
// Compression helpers
// ---------------------------------------------------------------------------

void checkpoint_t::compress_file(const std::string &source_file,
                                 const std::string &dest_file) {
  std::string command;
  if (config.snapshot_compress) {
    command = "zip -qjm " + dest_file + " " + source_file;
  } else if (config.snapshot_compress_zstd) {
    command = "zstd --rm -T0 -q " + source_file;
  } else {
    return;
  }

  std::cerr << "command: " + command << std::endl;

  int ret = std::system(command.c_str());
  if (ret != 0) {
    std::cerr << "error: compress " << source_file << " failed." << std::endl;
    exit(ret);
  }
}

void checkpoint_t::decompress_file(const std::string &source_file,
                                   const std::string &output_dir) {
  std::string command;
  if (config.snapshot_compress) {
    command = "unzip -o " + source_file + " -d " + output_dir;
  } else if (config.snapshot_compress_zstd) {
    command = "zstd -dfq -T0 " + source_file + " --output-dir-flat=" + output_dir;
  } else {
    return;
  }

  std::cerr << "command: " + command << std::endl;

  int ret = std::system(command.c_str());
  if (ret != 0) {
    std::cerr << "error: decompress " << source_file << " failed." << std::endl;
    exit(ret);
  }
}

static inline void remove_file(const std::string &file) {
  int ret = std::remove(file.c_str());
  if (ret != 0) {
    std::cerr << "error: remove " << file << " failed." << std::endl;
    exit(ret);
  }
}

static inline void create_directory(const std::string &directory) {
  int ret = mkdir(directory.c_str(), 0777);
  if (ret != 0) {
    std::cerr << "error: mkdir " << directory << " failed." << std::endl;
    exit(ret);
  }
}

static inline void remove_directory(const std::string &directory) {
  int ret = rmdir(directory.c_str());
  if (ret != 0) {
    std::cerr << "error: rmdir " << directory << " failed." << std::endl;
    exit(ret);
  }
}

// ---------------------------------------------------------------------------
// Deserialize (load)
// ---------------------------------------------------------------------------

void checkpoint_t::cpu_deserialize(const char *load_name) {
  // Put processor in debug mode so checkpoint bootrom can write debug CSRs
  proc->get_state()->debug_mode = true;
  proc->set_privilege(PRV_M, false);

  // Load boot code to bootrom
  std::string bootrom_load_name(load_name);
  bootrom_load_name.append(".bootram");

  std::ifstream boot_fin(bootrom_load_name, std::ios::binary);
  if (!boot_fin.good()) {
    std::cerr << "can't find bootram: " << bootrom_load_name << std::endl;
    exit(-1);
  }

  std::stringstream boot_stream;
  boot_stream << boot_fin.rdbuf();
  std::string boot_image = boot_stream.str();
  std::vector<char> rom;
  rom.insert(rom.begin(), boot_image.begin(), boot_image.end());

  bootrom = std::make_unique<rom_device_t>(rom);
  bus->add_device(CHECKPOINT_BOOTROM_BASE, bootrom.get());

  if (!need_trampoline) return;

  // When using "none" as ELF, htif_t::start() does not call reset()/set_rom(),
  // so DEFAULT_RSTVEC (0x1000) has no boot code. Create a trampoline that jumps
  // to CHECKPOINT_BOOTROM_BASE so the processor can boot.
  const int trampoline_size = 8;
  uint32_t trampoline[trampoline_size] = {
    0x297,                                                // auipc  t0, 0x0
    0x28593 + (trampoline_size * 4 << 20),                // addi   a1, t0, &dtb
    0xf1402573,                                           // csrr   a0, mhartid
    (proc->get_isa().get_max_xlen() == 32) ?
      0x0182a283u :                                       // lw     t0, 24(t0)
      0x0182b283u,                                        // ld     t0, 24(t0)
    0x28067,                                              // jr     t0
    0,
    (uint32_t)(CHECKPOINT_BOOTROM_BASE & 0xffffffff),
    (uint32_t)((uint64_t)CHECKPOINT_BOOTROM_BASE >> 32)
  };
  for (int i = 0; i < trampoline_size; i++)
    trampoline[i] = to_le(trampoline[i]);

  std::vector<char> tramp_rom((char*)trampoline, (char*)trampoline + sizeof(trampoline));
  trampoline_rom = std::make_shared<rom_device_t>(tramp_rom);
  bus->add_device(DEFAULT_RSTVEC, trampoline_rom.get());

  // Restore HTIF tohost/fromhost addresses from .re_regs file
  std::string regs_load_name(load_name);
  regs_load_name.append(".re_regs");
  std::ifstream regs_fin(regs_load_name);
  if (regs_fin.good()) {
    auto *sim_ptr = static_cast<sim_t*>(sim);
    std::string line;
    while (std::getline(regs_fin, line)) {
      auto pos = line.find(':');
      if (pos == std::string::npos) continue;
      std::string key = line.substr(0, pos);
      std::string val = line.substr(pos + 1);
      if (key == "tohost_addr")
        sim_ptr->set_tohost_addr(strtoull(val.c_str(), nullptr, 16));
      else if (key == "fromhost_addr")
        sim_ptr->set_fromhost_addr(strtoull(val.c_str(), nullptr, 16));
    }
    regs_fin.close();
    std::cerr << "NOTE: restored HTIF addresses: tohost=0x" << std::hex
              << sim_ptr->get_tohost_addr() << " fromhost=0x"
              << sim_ptr->get_fromhost_addr() << std::endl;
  }
}

#define MAINRAM_BASE   ".mainram"
#define MAINRAM_ZIP    ".mainram.zip"
#define MAINRAM_ZST    ".mainram.zst"

void checkpoint_t::ram_deserialize(const char *load_name) {
  bool mainram_zip_file_exist = false;
  bool mainram_zst_file_exist = false;
  uint8_t found_mainram = 0;

  std::list<std::string> mainram_types = {MAINRAM_BASE, MAINRAM_ZIP, MAINRAM_ZST};
  std::list<std::string> missing_mainram_names;
  std::string mainram_load_name;
  std::ifstream main_fin;

  for (const auto& type : mainram_types) {
    mainram_load_name = load_name + type;
    main_fin.open(mainram_load_name, std::ios::binary);
    if (main_fin.good()) {
      std::cerr << "found mainram: " << mainram_load_name << std::endl;
      found_mainram++;
      if (type == MAINRAM_ZIP)
        mainram_zip_file_exist = true;
      if (type == MAINRAM_ZST)
        mainram_zst_file_exist = true;
    } else {
      missing_mainram_names.push_back(mainram_load_name);
    }
    main_fin.close();
  }

  if (found_mainram != 1) {
    if (found_mainram == 0) {
      for (const auto& missing_name : missing_mainram_names)
        std::cerr << "not found mainram: " << missing_name << std::endl;
    }
    std::cerr << "found " << (int)found_mainram << " mainram files" << std::endl;
    exit(-1);
  }

  mainram_load_name = load_name;
  mainram_load_name.append(MAINRAM_BASE);

  std::string temp_directory_name;
  if (mainram_zip_file_exist || mainram_zst_file_exist) {
    config.snapshot_compress = mainram_zip_file_exist;
    config.snapshot_compress_zstd = mainram_zst_file_exist;
    temp_directory_name = "/tmp/spike-" + std::to_string(getpid());
    create_directory(temp_directory_name);
    std::string mainram_compress_name = mainram_load_name +
        (mainram_zip_file_exist ? std::string(".zip") : std::string(".zst"));
    decompress_file(mainram_compress_name, temp_directory_name);
    auto pos = mainram_load_name.rfind('/');
    std::string load_file_name = (pos != std::string::npos)
        ? mainram_load_name.substr(pos + 1) : mainram_load_name;
    mainram_load_name = temp_directory_name + "/" + load_file_name;
  }

  main_fin.open(mainram_load_name, std::ios::binary);
  if (!main_fin.good()) {
    std::cerr << "can't find mainram: " << mainram_load_name << std::endl;
    exit(-1);
  }

  main_fin.seekg(0, std::ios::end);
  std::streampos fileSize = main_fin.tellg();
  main_fin.seekg(0, std::ios::beg);

  std::vector<uint8_t> buffer(fileSize);
  main_fin.read(reinterpret_cast<char *>(buffer.data()), fileSize);

  sim->mmio_store(mainram_base, fileSize, buffer.data());

  if (config.snapshot_compress || config.snapshot_compress_zstd) {
    remove_file(mainram_load_name);
    remove_directory(temp_directory_name);
  }
}

// ---------------------------------------------------------------------------
// Serialize (save)
// ---------------------------------------------------------------------------

void checkpoint_t::save_regs_file(const char *save_name) {
  std::string regs_save_name(save_name);
  regs_save_name.append(".re_regs");

  std::ofstream regs_save_fout(regs_save_name);
  if (!regs_save_fout.is_open()) {
    std::cerr << "error: create regs file " << regs_save_name << " failed"
              << std::endl;
    exit(-1);
  }

  state_t *state = proc->get_state();

  regs_save_fout << "# spike serialization file" << std::endl;
  regs_save_fout << "pc:0x" << std::hex << state->pc << std::endl;

  for (int i = 0; i < 32; i++) {
    regs_save_fout << "reg_x" << std::dec << i << ":" << std::hex
                   << state->XPR[i] << std::endl;
  }

  if (proc->get_flen()) {
    for (int i = 0; i < 32; i++) {
      regs_save_fout << "reg_f" << std::dec << i << ":" << std::hex
                     << state->FPR[i].v[0] << std::endl;
    }
    regs_save_fout << "fflags:" << state->fflags->read() << std::endl;
    regs_save_fout << "frm:" << state->frm->read() << std::endl;
  }

  if (proc->extension_enabled('V')) {
    for (int r = 0; r < 32; r++) {
      const int vlen = (int)(proc->VU.get_vlen()) >> 3;
      const int elen = (int)(proc->VU.get_elen()) >> 3;
      const int num_elem = vlen / elen;

      regs_save_fout << "reg_v" << std::dec << std::setw(2) << std::setfill('0')
                     << r << ":" << std::endl;
      for (int e = num_elem - 1; e >= 0; --e) {
        uint64_t val;
        switch (elen) {
        case 8:
          val = proc->VU.elt<uint64_t>(r, e);
          regs_save_fout << std::dec << "[" << e << "]: 0x" << std::hex
                         << std::setfill('0') << std::setw(16) << val << "  ";
          break;
        case 4:
          val = proc->VU.elt<uint32_t>(r, e);
          regs_save_fout << std::dec << "[" << e << "]: 0x" << std::hex
                         << std::setfill('0') << std::setw(8) << (uint32_t)val
                         << "  ";
          break;
        case 2:
          val = proc->VU.elt<uint16_t>(r, e);
          regs_save_fout << std::dec << "[" << e << "]: 0x" << std::hex
                         << std::setfill('0') << std::setw(8) << (uint16_t)val
                         << "  ";
          break;
        case 1:
          val = proc->VU.elt<uint8_t>(r, e);
          regs_save_fout << std::dec << "[" << e << "]: 0x" << std::hex
                         << std::setfill('0') << std::setw(8)
                         << (int)(uint8_t)val << "  ";
          break;
        default:
          assert(0);
        }
        regs_save_fout << std::endl;
      }
    }

    regs_save_fout << "vstart:" << proc->VU.vstart->read() << std::endl;
    regs_save_fout << "vxsat:" << proc->VU.vxsat->read() << std::endl;
    regs_save_fout << "vxrm:" << proc->VU.vxrm->read() << std::endl;
    regs_save_fout << "vcsr:" << proc->get_state()->csrmap[CSR_VCSR]->read()
                   << std::endl;
    regs_save_fout << "vl:" << proc->VU.vl->read() << std::endl;
    regs_save_fout << "vtype:" << proc->VU.vtype->read() << std::endl;
    regs_save_fout << "vlenb:" << proc->get_state()->csrmap[CSR_VLENB]->read()
                   << std::endl;
  }

  const char *priv_str = "USHM";
  regs_save_fout << "priv:" << priv_str[state->prv] << std::endl;

  regs_save_fout << "mstatus:" << std::hex << state->mstatus->read()
                 << std::endl;
  regs_save_fout << "mtvec:" << std::hex << state->csrmap[CSR_MTVEC]->read()
                 << std::endl;
  regs_save_fout << "mscratch:" << std::hex
                 << state->csrmap[CSR_MSCRATCH]->read() << std::endl;
  regs_save_fout << "mepc:" << std::hex << state->csrmap[CSR_MEPC]->read()
                 << std::endl;
  regs_save_fout << "mcause:" << std::hex << state->csrmap[CSR_MCAUSE]->read()
                 << std::endl;
  regs_save_fout << "mtval:" << std::hex << state->csrmap[CSR_MTVAL]->read()
                 << std::endl;

  regs_save_fout << "misa:" << std::hex << state->misa->read() << std::endl;
  regs_save_fout << "mie:" << std::hex << state->csrmap[CSR_MIE]->read()
                 << std::endl;
  regs_save_fout << "mip:" << std::hex << state->csrmap[CSR_MIP]->read()
                 << std::endl;
  regs_save_fout << "medeleg:" << std::hex << state->csrmap[CSR_MEDELEG]->read()
                 << std::endl;
  regs_save_fout << "mideleg:" << std::hex << state->csrmap[CSR_MIDELEG]->read()
                 << std::endl;
  regs_save_fout << "mcounteren:" << std::hex
                 << state->csrmap[CSR_MCOUNTEREN]->read() << std::endl;
  regs_save_fout << "mcountinhibit:" << std::hex
                 << state->csrmap[CSR_MCOUNTINHIBIT]->read() << std::endl;
  regs_save_fout << "tselect:" << std::hex << state->csrmap[CSR_TSELECT]->read()
                 << std::endl;

  regs_save_fout << "stvec:" << std::hex << state->csrmap[CSR_STVEC]->read()
                 << std::endl;
  regs_save_fout << "sscratch:" << std::hex
                 << state->csrmap[CSR_SSCRATCH]->read() << std::endl;
  regs_save_fout << "sepc:" << std::hex << state->csrmap[CSR_SEPC]->read()
                 << std::endl;
  regs_save_fout << "scause:" << std::hex << state->csrmap[CSR_SCAUSE]->read()
                 << std::endl;
  regs_save_fout << "stval:" << std::hex << state->csrmap[CSR_STVAL]->read()
                 << std::endl;
  regs_save_fout << "satp:" << std::hex << state->csrmap[CSR_SATP]->read()
                 << std::endl;
  regs_save_fout << "scounteren:" << std::hex
                 << state->csrmap[CSR_SCOUNTEREN]->read() << std::endl;

  for (int i = 0; i < 4; i += 2) {
    regs_save_fout << "pmpcfg" << i << ":" << std::hex
                   << state->csrmap[CSR_PMPCFG0 + i]->read() << std::endl;
  }

  for (int i = 0; i < 16; i++) {
    regs_save_fout << "pmpaddr" << std::dec << i << ":" << std::hex
                   << state->csrmap[CSR_PMPADDR0 + i]->read() << std::endl;
  }

  // Save HTIF tohost/fromhost addresses for checkpoint restore
  auto *sim_ptr = static_cast<sim_t*>(sim);
  regs_save_fout << "tohost_addr:0x" << std::hex << sim_ptr->get_tohost_addr() << std::endl;
  regs_save_fout << "fromhost_addr:0x" << std::hex << sim_ptr->get_fromhost_addr() << std::endl;

  std::cerr << "NOTE: creating a new regs file: " << regs_save_name
            << std::endl;
  regs_save_fout.close();
}

// ---------------------------------------------------------------------------
// Boot ROM instruction generation helpers
// ---------------------------------------------------------------------------

static uint32_t create_csrrw(int rs, uint32_t csrn) {
  return 0x1073 | ((csrn & 0xFFF) << 20) | ((rs & 0x1F) << 15);
}

static uint32_t create_csrrs(int rd, uint32_t csrn) {
  return 0x2073 | ((csrn & 0xFFF) << 20) | ((rd & 0x1F) << 7);
}

static uint32_t create_auipc(int rd, uint32_t addr) {
  if (addr & 0x800)
    addr += 0x800;
  return 0x17 | ((rd & 0x1F) << 7) | ((addr >> 12) << 12);
}

static uint32_t create_addi(int rd, uint32_t addr) {
  uint32_t pos = addr & 0xFFF;
  return 0x13 | ((rd & 0x1F) << 7) | ((rd & 0x1F) << 15) |
         ((pos & 0xFFF) << 20);
}

static uint32_t create_seti(int rd, uint32_t data) {
  return 0x13 | ((rd & 0x1F) << 7) | ((data & 0xFFF) << 20);
}

static uint32_t create_ld(int rd, int rs1) {
  return 3 | ((rd & 0x1F) << 7) | (3 << 12) | ((rs1 & 0x1F) << 15);
}

static uint32_t create_sd(int rs1, int rs2) {
  return 0x23 | ((rs2 & 0x1F) << 20) | (3 << 12) | ((rs1 & 0x1F) << 15);
}

static uint32_t create_fld(int rd, int rs1) {
  return 7 | ((rd & 0x1F) << 7) | (0x3 << 12) | ((rs1 & 0x1F) << 15);
}

static uint32_t create_vtypei(int vlmul, int vsew, int vta, int vma) {
  return (vlmul & 0x7) | (vsew & 0x7) << 3 | (vta & 0x1) << 4 |
         (vma & 0x1) << 5;
}

static uint32_t create_vsetvli(int rd, int rs1, uint32_t vtypei) {
  return 87 | ((rd & 0x1F) << 7) | (7 << 12) | ((rs1 & 0x1F) << 15) |
         ((vtypei & 0x7FF) << 20);
}

static uint32_t create_vle8(int vd, int rs1) {
  return 7 | ((vd & 0x1F) << 7) | ((rs1 & 0x1F) << 15) | (1 << 25);
}

static void create_csr12_recovery(uint32_t *rom, uint32_t *code_pos,
                                  uint32_t csrn, uint16_t val) {
  rom[(*code_pos)++] = create_seti(1, val & 0xFFF);
  rom[(*code_pos)++] = create_csrrw(1, csrn);
}

static void create_csr64_recovery(uint32_t *rom, uint32_t *code_pos,
                                  uint32_t *data_pos, uint32_t csrn,
                                  uint64_t val) {
  uint32_t data_off = sizeof(uint32_t) * (*data_pos - *code_pos);

  rom[(*code_pos)++] = create_auipc(1, data_off);
  rom[(*code_pos)++] = create_addi(1, data_off);
  rom[(*code_pos)++] = create_ld(1, 1);
  rom[(*code_pos)++] = create_csrrw(1, csrn);

  rom[(*data_pos)++] = val & 0xFFFFFFFF;
  rom[(*data_pos)++] = val >> 32;
}

static void create_reg_recovery(uint32_t *rom, uint32_t *code_pos,
                                uint32_t *data_pos, int rn, uint64_t val) {
  uint32_t data_off = sizeof(uint32_t) * (*data_pos - *code_pos);

  rom[(*code_pos)++] = create_auipc(rn, data_off);
  rom[(*code_pos)++] = create_addi(rn, data_off);
  rom[(*code_pos)++] = create_ld(rn, rn);

  rom[(*data_pos)++] = val & 0xFFFFFFFF;
  rom[(*data_pos)++] = val >> 32;
}

static void create_vreg_recovery(uint32_t *rom, uint32_t *code_pos,
                                 uint32_t *data_pos, int vn,
                                 uint32_t *vreg_data, uint32_t size) {
  uint32_t data_off = sizeof(uint32_t) * (*data_pos - *code_pos);

  rom[(*code_pos)++] = create_auipc(1, data_off);
  rom[(*code_pos)++] = create_addi(1, data_off);
  rom[(*code_pos)++] = create_vle8(vn, 1);

  for (uint32_t i = 0; i < size; i++) {
    rom[(*data_pos)++] = *vreg_data;
    vreg_data++;
  }
}

static void create_io64_recovery(uint32_t *rom, uint32_t *code_pos,
                                 uint32_t *data_pos, uint64_t addr,
                                 uint64_t val) {
  uint32_t data_off = sizeof(uint32_t) * (*data_pos - *code_pos);

  rom[(*code_pos)++] = create_auipc(1, data_off);
  rom[(*code_pos)++] = create_addi(1, data_off);
  rom[(*code_pos)++] = create_ld(1, 1);

  rom[(*data_pos)++] = addr & 0xFFFFFFFF;
  rom[(*data_pos)++] = addr >> 32;

  uint32_t data_off2 = sizeof(uint32_t) * (*data_pos - *code_pos);
  rom[(*code_pos)++] = create_auipc(2, data_off2);
  rom[(*code_pos)++] = create_addi(2, data_off2);
  rom[(*code_pos)++] = create_ld(2, 2);

  rom[(*code_pos)++] = create_sd(1, 2);

  rom[(*data_pos)++] = val & 0xFFFFFFFF;
  rom[(*data_pos)++] = val >> 32;
}

static void create_hang_nonzero_hart(uint32_t *rom, uint32_t *code_pos,
                                     [[maybe_unused]] uint32_t *data_pos) {
  rom[(*code_pos)++] = 0xf1402573; // csrr   a0, mhartid
  rom[(*code_pos)++] = 0x00050663; // beqz   a0, 1f
  rom[(*code_pos)++] = 0x10500073; // wfi
  rom[(*code_pos)++] = 0xffdff06f; // j      0b
}

// ---------------------------------------------------------------------------
// Boot ROM creation
// ---------------------------------------------------------------------------

void checkpoint_t::create_boot_rom(const char *save_name) {
  std::string boot_rom_name(save_name);
  boot_rom_name.append(".bootram");

  std::ofstream boot_rom_fout(boot_rom_name);
  if (!boot_rom_fout.is_open()) {
    std::cerr << "Create bootram file " << boot_rom_name << " failed"
              << std::endl;
    exit(-1);
  }

  state_t *state = proc->get_state();

  if (state->prv == PRV_M &&
      state->pc >= CHECKPOINT_BOOTROM_BASE &&
      state->pc < (CHECKPOINT_BOOTROM_BASE + CHECKPOINT_BOOTROM_SIZE)) {
    std::cerr << "ERROR: could not checkpoint when running inside the checkpoint ROM."
              << std::endl;
    exit(-4);
  }
  std::cerr << "NOTE: creating a new boot rom:  " << boot_rom_name
            << std::endl;

  uint32_t rom[CHECKPOINT_BOOTROM_SIZE / 4] = {0};

  uint32_t code_pos = 0;
  uint32_t data_pos = 0xB00;
  uint32_t data_pos_start = data_pos;

  if (sim->get_harts().size() == 1) {
    create_hang_nonzero_hart(rom, &code_pos, &data_pos);
  }

  create_csr64_recovery(rom, &code_pos, &data_pos, 0x7b1,
                        state->pc); // DPC
  // dcsr: stopcount=1, stoptime=1, prv
  if (PRV_HS == state->prv) {
    std::cerr << "UNSUPPORTED Priv mode (no hyper)" << std::endl;
    exit(-4);
  }
  create_csr12_recovery(rom, &code_pos, 0x7b0, 0x600 | state->prv);

  // mstatus & misa first (some CSR writes depend on mstatus state)
  create_csr64_recovery(rom, &code_pos, &data_pos, 0x300,
                        state->mstatus->read());
  create_csr64_recovery(rom, &code_pos, &data_pos, 0x301,
                        state->misa->read());

  // FPU state
  if (state->sstatus->enabled(SSTATUS_FS)) {
    create_csr12_recovery(rom, &code_pos, 0x001, state->fflags->read());
    create_csr12_recovery(rom, &code_pos, 0x002, state->frm->read());
    create_csr12_recovery(rom, &code_pos, 0x003,
                          state->fflags->read() | (state->frm->read() << 5));

    for (int i = 0; i < 32; i++) {
      uint32_t data_off = sizeof(uint32_t) * (data_pos - code_pos);
      rom[code_pos++] = create_auipc(1, data_off);
      rom[code_pos++] = create_addi(1, data_off);
      rom[code_pos++] = create_fld(i, 1);

      rom[data_pos++] = static_cast<uint32_t>(state->FPR[i].v[0] & 0xFFFFFFFF);
      rom[data_pos++] = static_cast<uint32_t>(state->FPR[i].v[0] >> 32);
    }
  }

  // Vector state
  if (proc->extension_enabled('V') && state->sstatus->enabled(SSTATUS_VS)) {
    rom[code_pos++] = create_seti(1, -1);
    rom[code_pos++] = create_vsetvli(1, 1, create_vtypei(VLMUL8, VSEW8, 0, 0));

    uint32_t size = proc->VU.VLEN * 8 / 32;
    uint32_t *vreg_data = nullptr;

    vreg_data = &proc->VU.elt<uint32_t>(0, 0, false);
    create_vreg_recovery(rom, &code_pos, &data_pos, 0, vreg_data, size);
    vreg_data = &proc->VU.elt<uint32_t>(8, 0, false);
    create_vreg_recovery(rom, &code_pos, &data_pos, 8, vreg_data, size);
    vreg_data = &proc->VU.elt<uint32_t>(16, 0, false);
    create_vreg_recovery(rom, &code_pos, &data_pos, 16, vreg_data, size);
    vreg_data = &proc->VU.elt<uint32_t>(24, 0, false);
    create_vreg_recovery(rom, &code_pos, &data_pos, 24, vreg_data, size);

    create_csr64_recovery(rom, &code_pos, &data_pos, CSR_VSTART,
                          proc->VU.vstart->read());
    create_csr64_recovery(rom, &code_pos, &data_pos, CSR_VXSAT,
                          proc->VU.vxsat->read());
    create_csr64_recovery(rom, &code_pos, &data_pos, CSR_VXRM,
                          proc->VU.vxrm->read());
    create_csr64_recovery(rom, &code_pos, &data_pos, CSR_VCSR,
                          proc->get_state()->csrmap[CSR_VCSR]->read());

    volatile uint32_t vl = proc->VU.vl->read();
    volatile uint32_t vtype = proc->VU.vtype->read();
    rom[code_pos++] = create_seti(1, vl);
    rom[code_pos++] = create_vsetvli(1, 1, vtype);
  }

  // HPM counters
  for (int i = 0; i < 29; i++) {
    create_csr12_recovery(rom, &code_pos, 0xb03 + i, 0);
    create_csr64_recovery(rom, &code_pos, &data_pos, 0x323 + i,
                          state->csrmap[CSR_MHPMEVENT3 + i]->read());
  }

  create_csr64_recovery(rom, &code_pos, &data_pos, 0x7a0,
                        state->tselect->read());

  // Delegation, interrupt, and trap CSRs
  create_csr64_recovery(rom, &code_pos, &data_pos, 0x302,
                        state->medeleg->read());
  create_csr64_recovery(rom, &code_pos, &data_pos, 0x303,
                        state->mideleg->read());
  create_csr64_recovery(rom, &code_pos, &data_pos, 0x304,
                        state->mie->read());
  create_csr64_recovery(rom, &code_pos, &data_pos, 0x305, state->mtvec->read());
  create_csr64_recovery(rom, &code_pos, &data_pos, 0x105, state->stvec->read());
  create_csr12_recovery(rom, &code_pos, 0x320,
                        state->csrmap[CSR_MCOUNTINHIBIT]->read());
  create_csr12_recovery(rom, &code_pos, 0x306, state->mcounteren->read());
  create_csr12_recovery(rom, &code_pos, 0x106, state->scounteren->read());

  // PMP
  for (uint32_t i = 0; i < proc->n_pmp; ++i) {
    create_csr64_recovery(rom, &code_pos, &data_pos, 0x3B0 + i,
                          state->pmpaddr[i]->read());
  }
  for (uint32_t i = 0; i < proc->n_pmp / 4; i += 2) {
    create_csr64_recovery(rom, &code_pos, &data_pos, 0x3A0 + i,
                          state->csrmap[CSR_PMPCFG0 + i]->read());
  }

  // M-mode scratch/trap CSRs
  create_csr64_recovery(rom, &code_pos, &data_pos, 0x340,
                        state->csrmap[CSR_MSCRATCH]->read());
  create_csr64_recovery(rom, &code_pos, &data_pos, 0x341, state->mepc->read());
  create_csr64_recovery(rom, &code_pos, &data_pos, 0x342,
                        state->mcause->read());
  create_csr64_recovery(rom, &code_pos, &data_pos, 0x343, state->mtval->read());

  // S-mode scratch/trap CSRs
  create_csr64_recovery(rom, &code_pos, &data_pos, 0x140,
                        state->csrmap[CSR_SSCRATCH]->read());
  create_csr64_recovery(rom, &code_pos, &data_pos, 0x141, state->sepc->read());
  create_csr64_recovery(rom, &code_pos, &data_pos, 0x142,
                        state->scause->read());
  create_csr64_recovery(rom, &code_pos, &data_pos, 0x143, state->stval->read());

  create_csr64_recovery(rom, &code_pos, &data_pos, 0x344,
                        state->mip->read());

  // GPRs (x3..x31, skip x1 and x2 used as temporaries)
  for (int i = 3; i < 32; i++) {
    create_reg_recovery(rom, &code_pos, &data_pos, i, state->XPR[i]);
  }

  // CLINT timer
  void *fdt = (void *)dtb.c_str();
  reg_t clint_base;
  if (fdt_parse_clint(fdt, &clint_base, "riscv,clint0") != 0) {
    std::cerr << "Could not find the address of clint, failed to create bootrom"
              << std::endl;
    exit(-4);
  }

  create_io64_recovery(rom, &code_pos, &data_pos, clint_base + 0x4000,
                       clint->get_mtimecmp(0));
  create_csr64_recovery(rom, &code_pos, &data_pos, 0xb02,
                        state->minstret->read());
  create_csr64_recovery(rom, &code_pos, &data_pos, 0xb00,
                        state->mcycle->read());
  create_io64_recovery(rom, &code_pos, &data_pos, clint_base + 0xbff8,
                       clint->get_mtime());

  // Recover x1 and x2 last
  for (int i = 1; i < 3; i++) {
    create_reg_recovery(rom, &code_pos, &data_pos, i, state->XPR[i]);
  }

  // Save x1 to dscratch before satp write (changes addresses)
  rom[code_pos++] = create_csrrw(1, 0x7b2);
  create_csr64_recovery(rom, &code_pos, &data_pos, 0x180, state->satp->read());
  rom[code_pos++] = create_csrrs(1, 0x7b2);

  // dret - return from debug mode
  rom[code_pos++] = 0x7b200073;

  if ((CHECKPOINT_BOOTROM_SIZE / 4) <= data_pos || data_pos_start <= code_pos) {
    std::cerr << "ERROR: ROM is too small. ROM_SIZE should increase."
              << std::endl;
    std::cerr << "Current code_pos=" << code_pos << " data_pos=" << data_pos
              << std::endl;
    exit(-6);
  }

  boot_rom_fout.write(reinterpret_cast<char *>(rom), sizeof(rom));
  boot_rom_fout.close();
}

void checkpoint_t::cpu_serialize(const char *save_name) {
  save_regs_file(save_name);
  create_boot_rom(save_name);
}

void checkpoint_t::ram_serialize(const char *save_name) {
  std::string mainram_save_name(save_name);
  mainram_save_name.append(".mainram");

  auto mainram_pair = bus->find_device(mainram_base, 1);
  auto *mainram = dynamic_cast<mem_t *>(mainram_pair.second);

  std::ofstream mainram_fout(mainram_save_name, std::ios::binary);
  if (!mainram_fout.is_open()) {
    std::cerr << "Create mainram file " << mainram_save_name << " failed"
              << std::endl;
    exit(-1);
  }

  mainram->dump(mainram_fout);
  mainram_fout.close();

  if (config.snapshot_compress || config.snapshot_compress_zstd) {
    std::string mainram_compress_save_name = mainram_save_name +
        (config.snapshot_compress ? std::string(".zip") : std::string(".zst"));
    compress_file(mainram_save_name, mainram_compress_save_name);
    std::cerr << "NOTE: creating a new main ram:  "
              << mainram_compress_save_name << std::endl;
  } else {
    std::cerr << "NOTE: creating a new main ram:  " << mainram_save_name
              << std::endl;
  }
}
