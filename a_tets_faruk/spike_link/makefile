
# Automatically detect top module names from the tb directory
TB_DIR := src/rtl/tb
TB_MODULES := $(notdir $(basename $(wildcard $(TB_DIR)/*.sv)))

# Source and C++ files
SRC_FILES := $(wildcard src/rtl/design/*.sv)
CPP_FILES := $(wildcard src/cpp/*.cc)
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

INC_DIRS := -I/home/usr1/riscv-isa-sim/build2
INC_DIRS += -I/home/usr1/riscv-isa-sim/riscv
INC_DIRS += -I/home/usr1/riscv-isa-sim/fesvr
INC_DIRS += -I/home/usr1/riscv-isa-sim/
INC_DIRS += -I/home/usr1/riscv-isa-sim/softfloat
INC_DIRS += -I/home/usr1/riscv-isa-sim/fdt

# Define compile and run rules for each module dynamically
define TB_RULES

clean_$(1):
	@find obj_dir_$(1) -type f ! -name "$(1)__Dpi.h" -exec rm -f {} +

compile_$(1):
	verilator -CFLAGS "$(INC_DIRS)" --Mdir obj_dir_$(1) --binary +1800-2017ext+sv $(TB_DIR)/$(1).sv $(SRC_FILES) $(CPP_FILES) $(OBJ_FILES) $(LIBRARIES) --prefix $(1)  -o $(1).exe

.PHONY: compile_$(1)

run_with_compile_$(1): compile_$(1)
	./obj_dir_$(1)/$(1).exe

run_$(1):
	./obj_dir_$(1)/$(1).exe

endef

# Generate rules
$(foreach tb,$(TB_MODULES),$(eval $(call TB_RULES,$(tb))))

