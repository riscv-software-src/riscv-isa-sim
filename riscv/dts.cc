// See LICENSE for license details.

#include "dts.h"
#include <iostream>
#include <sstream>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

std::string make_dts(size_t insns_per_rtc_tick, size_t cpu_hz,
                     std::vector<processor_t*> procs,
                     std::vector<std::pair<reg_t, mem_t*>> mems)
{
  std::stringstream s;
  s << std::dec <<
         "/dts-v1/;\n"
         "\n"
         "/ {\n"
         "  #address-cells = <2>;\n"
         "  #size-cells = <2>;\n"
         "  compatible = \"ucbbar,spike-bare-dev\";\n"
         "  model = \"ucbbar,spike-bare\";\n"
         "  cpus {\n"
         "    #address-cells = <1>;\n"
         "    #size-cells = <0>;\n"
         "    timebase-frequency = <" << (cpu_hz/insns_per_rtc_tick) << ">;\n";
  for (size_t i = 0; i < procs.size(); i++) {
    s << "    CPU" << i << ": cpu@" << i << " {\n"
         "      device_type = \"cpu\";\n"
         "      reg = <" << i << ">;\n"
         "      status = \"okay\";\n"
         "      compatible = \"riscv\";\n"
         "      riscv,isa = \"" << procs[i]->get_isa_string() << "\";\n"
         "      mmu-type = \"riscv," << (procs[i]->get_max_xlen() <= 32 ? "sv32" : "sv48") << "\";\n"
         "      clock-frequency = <" << cpu_hz << ">;\n"
         "      CPU" << i << "_intc: interrupt-controller {\n"
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
                   " 0x" << (m.second->size() >> 32) << " 0x" << (m.second->size() & (uint32_t)-1) << ">;\n"
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
  for (size_t i = 0; i < procs.size(); i++)
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

std::string dts_compile(const std::string& dts)
{
  // Convert the DTS to DTB
  int dts_pipe[2];
  pid_t dts_pid;

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
    execl(DTC, DTC, "-O", "dtb", 0);
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
