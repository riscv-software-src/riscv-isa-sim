// See LICENSE for license details.

#include "dts.h"
#include "libfdt.h"
#include "platform.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

static std::string make_dts(size_t insns_per_rtc_tick,
                            size_t cpu_hz, const cfg_t &cfg)
{
  std::pair<reg_t, reg_t> initrd_bounds = cfg.initrd_bounds();
  reg_t initrd_start = initrd_bounds.first;
  reg_t initrd_end = initrd_bounds.second;
  const char *bootargs = cfg.bootargs();
  const isa_parser_t *isa = cfg.get_isa_parser()();

  std::stringstream s;
  s << std::dec <<
         "/dts-v1/;\n"
         "\n"
         "/ {\n"
         "  #address-cells = <2>;\n"
         "  #size-cells = <2>;\n"
         "  compatible = \"ucbbar,spike-bare-dev\";\n"
         "  model = \"ucbbar,spike-bare\";\n"
         "  chosen {\n";
  if (initrd_start < initrd_end) {
    s << "    linux,initrd-start = <" << (size_t)initrd_start << ">;\n"
         "    linux,initrd-end = <" << (size_t)initrd_end << ">;\n";
    if (!cfg.bootargs.overridden())
      bootargs = "root=/dev/ram console=hvc0 earlycon=sbi";
  } else {
    if (!cfg.bootargs.overridden())
      bootargs = "console=hvc0 earlycon=sbi";
  }
    s << "    bootargs = \"";
  for (size_t i = 0; i < strlen(bootargs); i++) {
    if (bootargs[i] == '"')
    s << '\\' << bootargs[i];
    else
    s << bootargs[i];
  }
    s << "\";\n";
    s << "  };\n"
         "  cpus {\n"
         "    #address-cells = <1>;\n"
         "    #size-cells = <0>;\n"
         "    timebase-frequency = <" << (cpu_hz/insns_per_rtc_tick) << ">;\n";
  for (size_t i = 0; i < cfg.nprocs(); i++) {
    s << "    CPU" << i << ": cpu@" << i << " {\n"
         "      device_type = \"cpu\";\n"
         "      reg = <" << i << ">;\n"
         "      status = \"okay\";\n"
         "      compatible = \"riscv\";\n"
         "      riscv,isa = \"" << isa->get_isa_string() << "\";\n"
         "      riscv,priv = \"" << isa->get_priv_string() << "\";\n"
         "      mmu-type = \"riscv," << (isa->get_max_xlen() <= 32 ? "sv32" : "sv48") << "\";\n"
         "      riscv,pmpregions = <16>;\n"
         "      riscv,pmpgranularity = <4>;\n"
         "      clock-frequency = <" << cpu_hz << ">;\n"
         "      CPU" << i << "_intc: interrupt-controller {\n"
         "        #address-cells = <2>;\n"
         "        #interrupt-cells = <1>;\n"
         "        interrupt-controller;\n"
         "        compatible = \"riscv,cpu-intc\";\n"
         "      };\n"
         "    };\n";
  }
  s <<   "  };\n";
  for (const auto &m : cfg.mem_layout()) {
    s << std::hex <<
         "  memory@" << m.base << " {\n"
         "    device_type = \"memory\";\n"
         "    reg = <0x" << (m.base >> 32) << " 0x" << (m.base & (uint32_t)-1) <<
                   " 0x" << (m.size >> 16 >> 16) << " 0x" << (m.size & (uint32_t)-1) << ">;\n"
         "  };\n";
  }
  s <<   "  soc {\n"
         "    #address-cells = <2>;\n"
         "    #size-cells = <2>;\n"
         "    compatible = \"ucbbar,spike-bare-soc\", \"simple-bus\";\n"
         "    ranges;\n"
         "    clint@" << CLINT_BASE << " {\n"
         "      compatible = \"riscv,clint0\";\n"
         "      interrupts-extended = <" << std::dec;
  for (size_t i = 0; i < cfg.nprocs(); i++)
    s << "&CPU" << i << "_intc 3 &CPU" << i << "_intc 7 ";
  reg_t clintbs = CLINT_BASE;
  reg_t clintsz = CLINT_SIZE;
  s << std::hex << ">;\n"
         "      reg = <0x" << (clintbs >> 32) << " 0x" << (clintbs & (uint32_t)-1) <<
                     " 0x" << (clintsz >> 32) << " 0x" << (clintsz & (uint32_t)-1) << ">;\n"
         "    };\n"
         "  };\n"
         "  htif {\n"
         "    compatible = \"ucb,htif0\";\n"
         "  };\n"
         "};\n";
  return s.str();
}

static std::string dts_compile(const std::string& dts)
{
  // Convert the DTS to DTB
  int dts_pipe[2];
  pid_t dts_pid;

  fflush(NULL); // flush stdout/stderr before forking
  if (pipe(dts_pipe) != 0 || (dts_pid = fork()) < 0) {
    std::cerr << "Failed to fork dts child: " << strerror(errno) << std::endl;
    exit(1);
  }

  // Child process to output dts
  if (dts_pid == 0) {
    close(dts_pipe[0]);
    int step, len = dts.length();
    const char *buf = dts.c_str();
    for (int done = 0; done < len; done += step) {
      step = write(dts_pipe[1], buf+done, len-done);
      if (step == -1) {
        std::cerr << "Failed to write dts: " << strerror(errno) << std::endl;
        exit(1);
      }
    }
    close(dts_pipe[1]);
    exit(0);
  }

  pid_t dtb_pid;
  int dtb_pipe[2];
  if (pipe(dtb_pipe) != 0 || (dtb_pid = fork()) < 0) {
    std::cerr << "Failed to fork dtb child: " << strerror(errno) << std::endl;
    exit(1);
  }

  // Child process to output dtb
  if (dtb_pid == 0) {
    dup2(dts_pipe[0], 0);
    dup2(dtb_pipe[1], 1);
    close(dts_pipe[0]);
    close(dts_pipe[1]);
    close(dtb_pipe[0]);
    close(dtb_pipe[1]);
    execlp(DTC, DTC, "-O", "dtb", 0);
    std::cerr << "Failed to run " DTC ": " << strerror(errno) << std::endl;
    exit(1);
  }

  close(dts_pipe[1]);
  close(dts_pipe[0]);
  close(dtb_pipe[1]);

  // Read-out dtb
  std::stringstream dtb;

  int got;
  char buf[4096];
  while ((got = read(dtb_pipe[0], buf, sizeof(buf))) > 0) {
    dtb.write(buf, got);
  }
  if (got == -1) {
    std::cerr << "Failed to read dtb: " << strerror(errno) << std::endl;
    exit(1);
  }
  close(dtb_pipe[0]);

  // Reap children
  int status;
  waitpid(dts_pid, &status, 0);
  if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
    std::cerr << "Child dts process failed" << std::endl;
    exit(1);
  }
  waitpid(dtb_pid, &status, 0);
  if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
    std::cerr << "Child dtb process failed" << std::endl;
    exit(1);
  }

  return dtb.str();
}

static int fdt_get_node_addr_size(const void *fdt, int node, reg_t *addr,
                                  unsigned long *size, const char *field)
{
  int parent, len, i;
  int cell_addr, cell_size;
  const fdt32_t *prop_addr, *prop_size;
  uint64_t temp = 0;

  parent = fdt_parent_offset(fdt, node);
  if (parent < 0)
    return parent;

  cell_addr = fdt_address_cells(fdt, parent);
  if (cell_addr < 1)
    return -ENODEV;

  cell_size = fdt_size_cells(fdt, parent);
  if (cell_size < 0)
    return -ENODEV;

  if (!field)
    return -ENODEV;

  prop_addr = (fdt32_t *)fdt_getprop(fdt, node, field, &len);
  if (!prop_addr)
    return -ENODEV;
  prop_size = prop_addr + cell_addr;

  if (addr) {
    for (i = 0; i < cell_addr; i++)
      temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
    *addr = temp;
  }
  temp = 0;

  if (size) {
    for (i = 0; i < cell_size; i++)
      temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
    *size = temp;
  }

  return 0;
}

static const char*
fdt_get_string_prop(const void *dtb, int offset,
                    const char *prop_name,
                    bool optional, const char *where)
{
  int len;
  const void *prop;
  prop = fdt_getprop(dtb, offset, prop_name, &len);
  if (!prop) {
    if (optional) return nullptr;

    std::cerr << "Cannot get " << prop_name << " property";
    if (where) std::cerr << " " << where;
    std::cerr << " in DTB.\n";
    exit(-1);
  }
  if (!len) {
    std::cerr << "The " << prop_name << " property";
    if (where) std::cerr << " " << where;
    std::cerr << " has zero length.\n";
    exit(-1);
  }
  return (const char*)prop;
}

cpu_dtb_t::cpu_dtb_t(size_t cpu_idx, const void *dtb, int offset)
  : dtb(dtb), offset(offset),
    num_pmp_regions(0), pmp_granularity(4)
{
  assert(dtb);
  assert(offset >= 0);

  std::ostringstream where_ss;
  where_ss << "for CPU " << cpu_idx;
  const char *where = where_ss.str().c_str();

  const char *device_type =
    fdt_get_string_prop(dtb, offset, "device_type", false, where);
  if (strcmp(device_type, "cpu")) {
    std::cerr << "The device_type property " << where
              << " is `" << device_type << "', not `cpu'.\n";
    exit(-1);
  }

  // Look up the ISA string / priv string for the CPU and parse it. If not
  // specified, these default to DEFAULT_ISA / DEFAULT_PRIV.
  const char *isa = fdt_get_string_prop(dtb, offset, "riscv,isa", true, where);
  if (!isa) isa = DEFAULT_ISA;

  const char *priv = fdt_get_string_prop(dtb, offset, "riscv,priv", true, where);
  if (!priv) priv = DEFAULT_PRIV;

  isa_parser.reset(new isa_parser_t(isa, priv));

  // Look up the number of PMP regions. If not specified, this defaults to 0
  fdt_get_node_addr_size(dtb, offset, &num_pmp_regions, NULL, "riscv,pmpregions");
  if (num_pmp_regions > 64) {
    std::cerr << "The riscv,pmpregions property " << where
              << "is " << num_pmp_regions
              << ", which is not supported (maximum 64).\n";
    exit(1);
  }

  // Look up PMP granularity. If not specified, this defaults to 4 (32-bit granularity)
  fdt_get_node_addr_size(dtb, offset, &pmp_granularity, NULL, "riscv,pmpgranularity");

  // Look up MMU type. If not specified, this is based on the ISA: IMPL_MMU_SV32 if
  // maximum xlen is <= 32; IMPL_MMU_SV48 otherwise.
  const char *mmu_type_str = fdt_get_string_prop(dtb, offset, "mmu-type", true, where);
  if (!mmu_type_str) {
    mmu_type = isa_parser->get_max_xlen() <= 32 ? IMPL_MMU_SV32 : IMPL_MMU_SV48;
  } else {
    if (strcmp(mmu_type_str, "riscv,sv32") == 0) {
      mmu_type = IMPL_MMU_SV32;
    } else if (strcmp(mmu_type_str, "riscv,sv39") == 0) {
      mmu_type = IMPL_MMU_SV39;
    } else if (strcmp(mmu_type_str, "riscv,sv48") == 0) {
      mmu_type = IMPL_MMU_SV48;
    } else if (strcmp(mmu_type_str, "riscv,sbare") == 0) {
      mmu_type = IMPL_MMU_SBARE;
    } else {
      std::cerr << "The mmu-type property " << where << " is `" << mmu_type_str
                << "', which isn't one of the supported values.\n";
      exit(-1);
    }
  }
}

void cpu_dtb_t::check_compatible(size_t i, const cfg_t &cfg) const
{
  // Check that if cfg gives an ISA string then it matches ours.
  if (cfg.isa.overridden() && (isa_parser->get_isa_string() != cfg.isa())) {
    std::cerr << "CPU " << i << " in the device tree has an ISA of `"
              << isa_parser->get_isa_string()
              << "', which doesn't match the command line ISA of `"
              << cfg.isa() << "'.\n";
    exit(-1);
  }

  // Similarly with a priv argument
  if (cfg.priv.overridden() && (isa_parser->get_priv_string() != cfg.priv())) {
    std::cerr << "CPU " << i << " in the device tree supports privilege modes "
              << isa_parser->get_priv_string()
              << ", which doesn't match the command line list: "
              << cfg.priv() << ".\n";
    exit(-1);
  }
}

devicetree_t::devicetree_t(std::string dtb, std::string dts, const char *src_path)
  : dtb(dtb), dts(dts)
{
  const void *fdt = dtb.c_str();
  int fdt_code = fdt_check_header(fdt);
  if (fdt_code) {
    std::cerr << "Failed to read DTB from ";
    if (!src_path) {
      std::cerr << "auto-generated DTS string";
    } else {
      std::cerr << "`" << src_path << "'";
    }
    std::cerr << ": " << fdt_strerror(fdt_code) << ".\n";
    exit(-1);
  }

  // Look up processors at the "/cpus" path. It's an error if there is
  // no such subtree.
  int cpus_offset = fdt_path_offset(fdt, "/cpus");
  if (cpus_offset < 0) {
    std::cerr << "DTB has no /cpus node.\n";
    exit(-1);
  }
  for (int off = fdt_first_subnode(fdt, cpus_offset);
       off >= 0;
       off = fdt_next_subnode(fdt, off)) {
    cpus.push_back(cpu_dtb_t(cpus.size(), fdt, off));
  }

  // It's also an error if there were no CPUs at all
  if (cpus.empty()) {
    std::cerr << "DTB has no entries in /cpus node.\n";
    exit(-1);
  }
}

devicetree_t devicetree_t::make(const char *dtb_path,
                                size_t insns_per_rtc_tick, size_t cpu_hz,
                                const cfg_t &cfg)
{
  if (dtb_path) {
    std::ifstream fin(dtb_path, std::ios::binary);
    if (!fin.good()) {
      std::cerr << "can't find dtb file: " << dtb_path << std::endl;
      exit(-1);
    }

    std::stringstream strstream;
    strstream << fin.rdbuf();

    devicetree_t ret(strstream.str(), "", dtb_path);
    ret.check_compatible(cfg);
    return ret;
  }

  std::string dts = make_dts(insns_per_rtc_tick, cpu_hz, cfg);
  std::string dtb = dts_compile(dts);

  return devicetree_t(dtb, dts, dtb_path);
}

void devicetree_t::check_compatible(const cfg_t &cfg) const
{
  // Check that if nprocs was specified in the base config then it matches the
  // number of processors in the DTB.
  if (cfg.nprocs.overridden() && (cpus.size() != cfg.nprocs())) {
    std::cerr << "Number of processors specified on command line was "
              << cfg.nprocs()
              << ", but the DTB file contains " << cpus.size()
              << ".\n";
    exit(-1);
  }

  for (size_t i = 0; i < cpus.size(); ++i) {
    cpus[i].check_compatible(i, cfg);
  }
}

int devicetree_t::find_clint(reg_t *clint_addr, const char *compatible) const
{
  assert(clint_addr);

  int nodeoffset, rc;

  nodeoffset = fdt_node_offset_by_compatible(dtb.c_str(), -1, compatible);
  if (nodeoffset < 0)
    return nodeoffset;

  rc = fdt_get_node_addr_size(dtb.c_str(), nodeoffset, clint_addr, NULL, "reg");
  if (rc < 0)
    return -ENODEV;

  return 0;
}
