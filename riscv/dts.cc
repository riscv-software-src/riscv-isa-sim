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

static int check_cpu_node(const void *fdt, int cpu_offset)
{
  int len;
  const void *prop;

  if (!fdt || cpu_offset < 0)
    return -EINVAL;

  prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
  if (!prop || !len)
    return -EINVAL;
  if (strncmp ((char *)prop, "cpu", strlen ("cpu")))
    return -EINVAL;

  return 0;
}

devicetree_t::devicetree_t(std::string dtb, std::string dts, const char *src_path)
  : dtb(dtb), dts(dts)
{
  int fdt_code = fdt_check_header(dtb.c_str());
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

    return devicetree_t(strstream.str(), "", dtb_path);
  }

  std::string dts = make_dts(insns_per_rtc_tick, cpu_hz, cfg);
  std::string dtb = dts_compile(dts);

  return devicetree_t(dtb, dts, dtb_path);
}

int devicetree_t::get_offset(const char *path) const
{
  return fdt_path_offset(dtb.c_str(), path);
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

int devicetree_t::get_pmp_num(int cpu_offset, reg_t *pmp_num) const
{
  assert(pmp_num);

  int rc;

  if ((rc = check_cpu_node(dtb.c_str(), cpu_offset)) < 0)
    return rc;

  rc = fdt_get_node_addr_size(dtb.c_str(), cpu_offset, pmp_num, NULL,
                              "riscv,pmpregions");
  if (rc < 0)
    return -ENODEV;

  return 0;
}

int devicetree_t::get_pmp_alignment(int cpu_offset, reg_t *pmp_align) const
{
  assert(pmp_align);

  int rc;

  if ((rc = check_cpu_node(dtb.c_str(), cpu_offset)) < 0)
    return rc;

  rc = fdt_get_node_addr_size(dtb.c_str(), cpu_offset, pmp_align, NULL,
                              "riscv,pmpgranularity");
  if (rc < 0)
    return -ENODEV;

  return 0;
}

int devicetree_t::get_mmu_type(int cpu_offset, const char **mmu_type) const
{
  assert(mmu_type);

  int len, rc;
  const void *prop;

  if ((rc = check_cpu_node(dtb.c_str(), cpu_offset)) < 0)
    return rc;

  prop = fdt_getprop(dtb.c_str(), cpu_offset, "mmu-type", &len);
  if (!prop || !len)
    return -EINVAL;

  *mmu_type = (const char *)prop;

  return 0;
}

int fdt_get_first_subnode(const void *fdt, int node)
{
  return fdt_first_subnode(fdt, node);
}

int fdt_get_next_subnode(const void *fdt, int node)
{
  return fdt_next_subnode(fdt, node);
}
