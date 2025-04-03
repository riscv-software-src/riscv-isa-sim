// See LICENSE for license details.

#include "config.h"
#include "dts.h"
#include "libfdt.h"
#include "platform.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

std::string make_dts(size_t insns_per_rtc_tick, size_t cpu_hz,
                     const cfg_t* cfg,
                     std::vector<std::pair<reg_t, abstract_mem_t*>> mems,
                     std::string device_nodes)
{
  reg_t initrd_start = cfg->initrd_bounds.first;
  reg_t initrd_end = cfg->initrd_bounds.second;
  const char* bootargs = cfg->bootargs;
  reg_t pmpregions = cfg->pmpregions;
  reg_t pmpgranularity = cfg->pmpgranularity;
  isa_parser_t isa(cfg->isa, cfg->priv);

  std::stringstream s;
  s << std::dec <<
         "/dts-v1/;\n"
         "\n"
         "/ {\n"
         "  #address-cells = <2>;\n"
         "  #size-cells = <2>;\n"
         "  compatible = \"ucbbar,spike-bare-dev\";\n"
         "  model = \"ucbbar,spike-bare\";\n"
         "  chosen {\n"
         "    stdout-path = &SERIAL0;\n";
  if (initrd_start < initrd_end) {
    s << "    linux,initrd-start = <" << (size_t)initrd_start << ">;\n"
         "    linux,initrd-end = <" << (size_t)initrd_end << ">;\n";
    if (!bootargs)
      bootargs = "root=/dev/ram " DEFAULT_KERNEL_BOOTARGS;
  } else {
    if (!bootargs)
      bootargs = DEFAULT_KERNEL_BOOTARGS;
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
    for (size_t i = 0; i < cfg->nprocs(); i++) {
    s << "    CPU" << i << ": cpu@" << i << " {\n"
         "      device_type = \"cpu\";\n"
         "      reg = <" << cfg->hartids[i] << ">;\n"
         "      status = \"okay\";\n"
         "      compatible = \"riscv\";\n"
         "      riscv,isa = \"" << isa.get_isa_string() << "\";\n"
         "      mmu-type = \"riscv," << (isa.get_max_xlen() <= 32 ? "sv32" : "sv57") << "\";\n"
         "      riscv,pmpregions = <" << pmpregions << ">;\n"
         "      riscv,pmpgranularity = <" << pmpgranularity << ">;\n"
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
  for (auto& m : mems) {
    s << std::hex <<
         "  memory@" << m.first << " {\n"
         "    device_type = \"memory\";\n"
         "    reg = <0x" << (m.first >> 32) << " 0x" << (m.first & (uint32_t)-1) <<
                   " 0x" << (m.second->size() >> 16 >> 16) << " 0x" << (m.second->size() & (uint32_t)-1) << ">;\n"
         "  };\n";
  }
  s <<   "  soc {\n"
         "    #address-cells = <2>;\n"
         "    #size-cells = <2>;\n"
         "    compatible = \"ucbbar,spike-bare-soc\", \"simple-bus\";\n"
         "    ranges;\n"
    << device_nodes
    <<   "  };\n"
         "  htif {\n"
         "    compatible = \"ucb,htif0\";\n"
         "  };\n"
         "};\n";
  return s.str();
}

static std::string dtc_compile(const std::string& dtc_input, bool compile)
{
  const char* input_type = compile ? "dts" : "dtb";
  const char* output_type = compile ? "dtb" : "dts";

  int dtc_input_pipe[2];
  pid_t dtc_input_pid;

  fflush(NULL); // flush stdout/stderr before forking
  if (pipe(dtc_input_pipe) != 0 || (dtc_input_pid = fork()) < 0) {
    std::cerr << "Failed to fork dtc_input child: " << strerror(errno) << std::endl;
    exit(1);
  }

  // Child process to output dtc_input
  if (dtc_input_pid == 0) {
    close(dtc_input_pipe[0]);
    int step, len = dtc_input.length();
    const char *buf = dtc_input.c_str();
    for (int done = 0; done < len; done += step) {
      step = write(dtc_input_pipe[1], buf+done, len-done);
      if (step == -1) {
        std::cerr << "Failed to write dtc_input: " << strerror(errno) << std::endl;
        exit(1);
      }
    }
    close(dtc_input_pipe[1]);
    exit(0);
  }

  pid_t dtc_output_pid;
  int dtc_output_pipe[2];
  if (pipe(dtc_output_pipe) != 0 || (dtc_output_pid = fork()) < 0) {
    std::cerr << "Failed to fork dtc_output child: " << strerror(errno) << std::endl;
    exit(1);
  }

  // Child process to output dtc_output
  if (dtc_output_pid == 0) {
    dup2(dtc_input_pipe[0], 0);
    dup2(dtc_output_pipe[1], 1);
    close(dtc_input_pipe[0]);
    close(dtc_input_pipe[1]);
    close(dtc_output_pipe[0]);
    close(dtc_output_pipe[1]);
    execlp(DTC, DTC, "-O", output_type, "-I", input_type, nullptr);
    std::cerr << "Failed to run " DTC ": " << strerror(errno) << std::endl;
    exit(1);
  }

  close(dtc_input_pipe[1]);
  close(dtc_input_pipe[0]);
  close(dtc_output_pipe[1]);

  // Read-out dtc_output
  std::stringstream dtc_output;

  int got;
  char buf[4096];
  while ((got = read(dtc_output_pipe[0], buf, sizeof(buf))) > 0) {
    dtc_output.write(buf, got);
  }
  if (got == -1) {
    std::cerr << "Failed to read dtc_output: " << strerror(errno) << std::endl;
    exit(1);
  }
  close(dtc_output_pipe[0]);

  // Reap children
  int status;
  waitpid(dtc_input_pid, &status, 0);
  if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
    std::cerr << "Child dtc_input process failed" << std::endl;
    exit(1);
  }
  waitpid(dtc_output_pid, &status, 0);
  if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
    std::cerr << "Child dtc_output process failed" << std::endl;
    exit(1);
  }

  return dtc_output.str();
}

std::string dtb_to_dts(const std::string& dtc_input)
{
  return dtc_compile(dtc_input, false);
}

std::string dts_to_dtb(const std::string& dtc_input)
{
  return dtc_compile(dtc_input, true);
}

int fdt_get_node_addr_size(const void *fdt, int node, reg_t *addr,
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

int fdt_get_offset(const void *fdt, const char *field)
{
  return fdt_path_offset(fdt, field);
}

int fdt_get_first_subnode(const void *fdt, int node)
{
  return fdt_first_subnode(fdt, node);
}

int fdt_get_next_subnode(const void *fdt, int node)
{
  return fdt_next_subnode(fdt, node);
}

int fdt_parse_clint(const void *fdt, reg_t *clint_addr,
                    const char *compatible)
{
  int nodeoffset, rc;

  nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
  if (nodeoffset < 0)
    return nodeoffset;

  rc = fdt_get_node_addr_size(fdt, nodeoffset, clint_addr, NULL, "reg");
  if (rc < 0 || !clint_addr)
    return -ENODEV;

  return 0;
}

int fdt_parse_plic(const void *fdt, reg_t *plic_addr, uint32_t *ndev,
                   const char *compatible)
{
  int nodeoffset, len, rc;
  const fdt32_t *ndev_p;

  nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
  if (nodeoffset < 0)
    return nodeoffset;

  rc = fdt_get_node_addr_size(fdt, nodeoffset, plic_addr, NULL, "reg");
  if (rc < 0 || !plic_addr)
    return -ENODEV;

  ndev_p = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
  if (!ndev || !ndev_p)
    return -ENODEV;
  *ndev = fdt32_to_cpu(*ndev_p);

  return 0;
}

int fdt_parse_ns16550(const void *fdt, reg_t *ns16550_addr,
                      uint32_t *reg_shift, uint32_t *reg_io_width,
                      uint32_t* reg_int_id,
                      const char *compatible)
{
  int nodeoffset, len, rc;
  const fdt32_t *reg_p;

  nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
  if (nodeoffset < 0)
    return nodeoffset;

  rc = fdt_get_node_addr_size(fdt, nodeoffset, ns16550_addr, NULL, "reg");
  if (rc < 0 || !ns16550_addr)
    return -ENODEV;

  reg_p = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
  if (reg_shift) {
    if (reg_p) {
      *reg_shift = fdt32_to_cpu(*reg_p);
    } else {
      *reg_shift = NS16550_REG_SHIFT;
    }
  }

  reg_p = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
  if (reg_io_width) {
    if (reg_p) {
      *reg_io_width = fdt32_to_cpu(*reg_p);
    } else {
      *reg_io_width = NS16550_REG_IO_WIDTH;
    }
  }

  reg_p = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "interrupts", &len);
  if (reg_int_id) {
    if (reg_p) {
      *reg_int_id = fdt32_to_cpu(*reg_p);
    } else {
      *reg_int_id = NS16550_INTERRUPT_ID;
    }
  }

  return 0;
}

int fdt_parse_pmp_num(const void *fdt, int cpu_offset, reg_t *pmp_num)
{
  int rc;

  if ((rc = check_cpu_node(fdt, cpu_offset)) < 0)
    return rc;

  rc = fdt_get_node_addr_size(fdt, cpu_offset, pmp_num, NULL,
                              "riscv,pmpregions");
  if (rc < 0 || !pmp_num)
    return -ENODEV;

  return 0;
}

int fdt_parse_pmp_alignment(const void *fdt, int cpu_offset, reg_t *pmp_align)
{
  int rc;

  if ((rc = check_cpu_node(fdt, cpu_offset)) < 0)
    return rc;

  rc = fdt_get_node_addr_size(fdt, cpu_offset, pmp_align, NULL,
                              "riscv,pmpgranularity");
  if (rc < 0 || !pmp_align)
    return -ENODEV;

  return 0;
}

int fdt_parse_mmu_type(const void *fdt, int cpu_offset, const char **mmu_type)
{
  assert(mmu_type);

  int len, rc;
  const void *prop;

  if ((rc = check_cpu_node(fdt, cpu_offset)) < 0)
    return rc;

  prop = fdt_getprop(fdt, cpu_offset, "mmu-type", &len);
  if (!prop || !len)
    return -EINVAL;

  *mmu_type = (const char *)prop;

  return 0;
}

int fdt_parse_isa(const void *fdt, int cpu_offset, const char **isa)
{
  assert(isa);

  int len, rc;
  const void *prop;

  if ((rc = check_cpu_node(fdt, cpu_offset)) < 0)
    return rc;

  prop = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
  if (!prop || !len)
    return -EINVAL;

  *isa = (const char *)prop;

  return 0;
}

int fdt_parse_hartid(const void *fdt, int cpu_offset, uint32_t *hartid)
{
  int len, rc;
  const void *prop;
  const fdt32_t *val;

  if ((rc = check_cpu_node(fdt, cpu_offset)) < 0)
    return rc;

  val = (fdt32_t*) fdt_getprop(fdt, cpu_offset, "reg", &len);
  if (!val || len < (int) sizeof(fdt32_t))
    return -EINVAL;

  if (len > (int) sizeof(fdt32_t))
    val++;

  if (hartid)
    *hartid = fdt32_to_cpu(*val);

  return 0;
}
