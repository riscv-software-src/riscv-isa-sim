".PHONY: clean $(TB_MODULES:%=compile_%) $(TB_MODULES:%=run_%)

# Automatically detect top module names from the tb directory
TB_MODULES := $(notdir $(basename $(wildcard src/rtl/tb/*.sv)))

# Source and C++ files
SRC_FILES := $(wildcard src/rtl/src/*.sv)
CPP_FILES := $(wildcard src/cpp/*.cc)

# Default target
# all: $(TB_MODULES:%=compile_%)

# Rule to clean the obj_dir
clean:
	rm -rf obj_dir/*

# Define compile and run rules for each module dynamically
define TB_RULES
compile_$(1): obj_dir/$(1).exe

obj_dir/$(1).exe: src/rtl/tb/$(1).sv $(SRC_FILES) $(CPP_FILES)
	verilator --binary +1800-2017ext+sv $$< $(SRC_FILES) $(CPP_FILES) --prefix $(1)  -o $(1).exe

run_$(1): compile_$(1)
	./obj_dir/$(1).exe
endef

# Generate rules
$(foreach tb,$(TB_MODULES),$(eval $(call TB_RULES,$(tb))))

# Rule to run all testbenches
# run: $(TB_MODULES:%=run_%)

# g++     stateful.o verilated.o verilated_dpi.o verilated_timing.o verilated_threads.o tb_1__ALL.a    -pthread -lpthread -latomic   -o tb_1"