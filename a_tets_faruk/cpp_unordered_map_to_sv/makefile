".PHONY: clean $(TB_MODULES:%=compile_%) $(TB_MODULES:%=run_%)

# Automatically detect top module names from the tb directory
TB_MODULES := $(notdir $(basename $(wildcard src/rtl/tb/*.sv)))

# Source and C++ files
SRC_FILES := $(wildcard src/rtl/design/*.sv)
CPP_FILES := $(wildcard src/cpp/*.cc)



# Define compile and run rules for each module dynamically
define TB_RULES

clean_$(1):
	@find obj_dir_$(1) -type f ! -name "$(1)__Dpi.h" -exec rm -f {} +

compile_$(1): obj_dir_$(1)/$(1).exe

obj_dir_$(1)/$(1).exe: src/rtl/tb/$(1).sv $(SRC_FILES) $(CPP_FILES)
	verilator --Mdir obj_dir_$(1) --binary +1800-2017ext+sv $$< $(SRC_FILES) $(CPP_FILES) --prefix $(1)  -o $(1).exe

run_$(1): compile_$(1)
	./obj_dir_$(1)/$(1).exe
endef

# Generate rules
$(foreach tb,$(TB_MODULES),$(eval $(call TB_RULES,$(tb))))

