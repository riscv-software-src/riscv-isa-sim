# Makefile for program model example

XLEN ?= 64
VLEN ?= 1024
RISCV ?= /home/scratch.soberl_maxwell/arch1/riscv-tools-rvv/build
RISCV_TOOL = $(RISCV)/bin
RISCV_TEST ?= /home/scratch.soberl_maxwell/arch1/riscv-tools-rvv/riscv-tests

SPIKE_PATH = /home/scratch.soberl_maxwell/arch1/riscv-tools-p1.12/build/bin

SSP_OPT ?=
PERF ?= 0

COMMON_PATH = $(RISCV_TEST)/benchmarks/common
LIB_PATH = .
# ../ctests/nvrvv_lib.c
COMMON_FILES = \
	$(LIB_PATH)/crt.S \
	$(LIB_PATH)/syscalls.c 

TEST_PATH = ./gengen_src/outputs

ALL_TEST = $(basename $(notdir $(wildcard $(TEST_PATH)/*.c)))
DEV_TEST = test_pmp_csr_1_lock00_rlb0_mmwp1_mml0_sec_06
OBJECTS ?= $(ALL_TEST)

# NEVER enable 'C' because pc + 4 is used in test code.
# -ffast-math -fno-common -fno-builtin-printf
CFLAGS = -march=rv64imafd -O2 -I . -I ./$(LIB_PATH) -I ../softfloat -I ../riscv \
	-I $(RISCV_TEST)/env -I $(RISCV_TEST)/benchmarks/common \
	-fno-builtin-printf -fdata-sections -fno-section-anchors $(SSP_OPT)
LDFLAGS = -mcmodel=medany -static -nostdlib -nostartfiles -lm -lgcc \
	-T $(LIB_PATH)/mseccfg_test.ld  -Wl,-M -Wl,-Map=link.log
	
# must enable 'C', maybe used in pk
# 8M for TCM memories
# 16M for L2 memories
SIM_ISA = --isa=RV64IMAFDC

default:
	@echo "make gen, to generate all test cases with gengen"
	@echo "make all, to run all test cases"
	@echo "set OBJECTS variant to select specified test case"
	
gen:
	cd gengen_src; $(MAKE); $(MAKE) gen;
	
$(OBJECTS):
	$(RISCV_TOOL)/riscv64-unknown-elf-gcc $(CFLAGS) $(TEST_PATH)/$@.c $(COMMON_FILES) $(LDFLAGS) -o a.out 
	@echo Running $(TEST_PATH)/$@.c
	@$(RISCV_TOOL)/riscv64-unknown-elf-objdump -d a.out > a.ss
	@$(RISCV_TOOL)/riscv64-unknown-elf-objdump --disassemble=target_foo a.out >> a.ss
ifeq ($(PERF), 0)
	$(SPIKE_PATH)/spike $(SIM_ISA) -m0x100000:0x200000 a.out
	/home/scratch.soberl_maxwell/arch1/sail_all/sail-riscv/c_emulator/riscv_sim_RV64 --enable-pmp -V a.out > tmp.log 2>&1; grep SUCCESS tmp.log
	@-grep FAILURE tmp.log
#	$(RISCV_TOOL)/spike $(SIM_ISA) -l a.out > $@_pc.log 2>&1
#	sed -i '0,/ nop/d' $@_pc.log
#	sed -i '/ nop/q' $@_pc.log
endif
	
all: $(OBJECTS)

clean:
	rm *.s *.o *.i *.ss *.out *.log *.bin
	
log:
	$(SPIKE_PATH)/spike $(SIM_ISA) -m0x100000:0x200000 -l a.out > 1.log 2>&1
	/home/scratch.soberl_maxwell/arch1/sail_all/sail-riscv/c_emulator/riscv_sim_RV64 --enable-pmp a.out > 2.log 2>&1
	
env:
	echo $(ALL_TEST)
	

.PHONY: all $(OBJECTS) clean
