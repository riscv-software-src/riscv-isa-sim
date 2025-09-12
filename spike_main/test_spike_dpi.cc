/*
gcc -g -L. -Wl,--export-dynamic -L/usr/lib/x86_64-linux-gnu  -Wl,-rpath,/usr/local/lib \
      ../build/extension.o test_spike_dpi.cc  -o test_spike_dpi.elf \
      ../build/libspike_main.a  ../build/libriscv.a  ../build/libdisasm.a  ../build/libsoftfloat_8.a \
      ../build/libsoftfloat.a  ../build/libfesvr.a  ../build/libfdt.a \
      -lpthread -lboost_regex -lboost_system -static-libgcc -static-libstdc++ -lstdc++ -I/usr/share/verilator/include/vltstd
*/
#include <iostream>
#include <string>
#include <cstdint>
#include <svdpi.h>
extern "C" {
    void spike_set_dtb_file(const char* path);
    void spike_setup(long long, const char*);
    void start_execution();
    int  exit_code();
    uint64_t spike_get_pc(unsigned);
    void spike_dump_registers(unsigned);
    void spike_dump_csrs(unsigned);
    uint64_t spike_get_reg(unsigned, unsigned);
    uint64_t spike_get_freg(unsigned, unsigned);
    void spike_get_vreg(unsigned, unsigned, const svOpenArrayHandle);
    uint64_t spike_get_csr(unsigned, const char*);
    void do_step(unsigned long long, unsigned);
}
int spike(int argc, char** argv);


int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " \"spike arguments\"" << std::endl;
        return 1;
    }

    std::string args;
    for (int i = 1; i < argc; ++i) {
        if (i > 1) args += ' ';
        args += argv[i];
    }
    spike_set_dtb_file("spike.dtb");
    spike_setup(0, args.c_str());

    //std::cout << "Initial PC: 0x" << std::hex << spike_get_pc(0) << std::dec << std::endl;
    //spike_dump_registers(0);
    //spike_dump_csrs(0);

    //std::cout << "x1 = 0x" << std::hex << spike_get_reg(0, 1) << std::dec << std::endl;
    //std::cout << "f0 = 0x" << std::hex << spike_get_freg(0, 0) << std::dec << std::endl;
    //svBitVecVal vec[8] = {0};
    //spike_get_vreg(0, 0, vec);
    //std::cout << "v0[0]=0x" << std::hex << vec[0] << std::dec << std::endl;
    //std::cout << "mstatus=0x" << std::hex << spike_get_csr(0, "mstatus") << std::dec << std::endl;

    for (int i=0; i<5000; i++) {
        for (int j=0; j<3; j++) {
            do_step(1, j);
        }
    }
    //std::cout << "After 1 step PC: 0x" << std::hex << (unsigned int) spike_get_pc(0) << std::dec << std::endl;

    //start_execution();
    std::cout << "Simulation exited with code " << exit_code() << std::endl;
    return 0;
}