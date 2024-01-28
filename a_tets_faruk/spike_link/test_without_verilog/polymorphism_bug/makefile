

# Source and C++ files
CPP_FILES := $(wildcard ../cpp/*.cc)
LIBRARIES :=
# LIBRARIES += $(wildcard /home/usr1/riscv-isa-sim/build/*.a)
# LIBRARIES += $(wildcard /home/usr1/riscv-isa-sim/build/*.so)
# LIBRARIES := /home/usr1/riscv-isa-sim/build/libfesvr.a
# LIBRARIES += /home/usr1/riscv-isa-sim/build/libriscv.a
# LIBRARIES += /home/usr1/riscv-isa-sim/build/libsoftfloat.a
LIBRARIES += /home/usr1/riscv-isa-sim/build/libspike_dasm.a
# LIBRARIES += /home/usr1/riscv-isa-sim/build/libfdt.a
# LIBRARIES += /home/usr1/riscv-isa-sim/build/libfdt.so
# LIBRARIES += /home/usr1/riscv-isa-sim/build/libcustomext.so
# LIBRARIES += /home/usr1/riscv-isa-sim/build/libsoftfloat.so
LIBRARIES += /home/usr1/riscv-isa-sim/build/libriscv.so

# precompiled header'lar icin miydi bu asagidaki?
INC_DIRS := -I/home/usr1/riscv-isa-sim/build
INC_DIRS += -I/home/usr1/riscv-isa-sim/riscv
INC_DIRS += -I/home/usr1/riscv-isa-sim/fesvr
INC_DIRS += -I/home/usr1/riscv-isa-sim/
INC_DIRS += -I/home/usr1/riscv-isa-sim/softfloat
INC_DIRS += -I/home/usr1/riscv-isa-sim/fdt

# add dependency creation rule 
.dependencies.d: $(CPP_FILES)
	g++ -MM $(INC_DIRS) $(CPP_FILES) > .dependencies.d

# include dependencies
-include .dependencies.d

# main with only htif.h
compile: $(CPP_FILES) $(LIBRARIES)
	g++  -o polm_bug.exe -O2 $(INC_DIRS) main.cc  $(LIBRARIES)
