# this makefile prepared by chatGPT:
# https://chat.openai.com/share/93d10b9a-8b83-4276-9686-f887e9c3c9a3
# .PHONY: $(TB_MODULES:%=compile_%) $(TB_MODULES:%=run_%)

# Automatically detect top module names from the tb directory
TB_DIR := src/rtl/tb
TB_MODULES := $(notdir $(basename $(wildcard src/rtl/tb/*.sv)))

# Source and C++ files
SRC_FILES := $(wildcard src/rtl/design/*.sv)
CPP_FILES := $(wildcard src/cpp/*.cc)



# Define compile and run rules for each module dynamically
define TB_RULES

clean_$(1):
	@find obj_dir_$(1) -type f ! -name "$(1)__Dpi.h" -exec rm -f {} +

compile_$(1): 
	verilator --Mdir obj_dir_$(1) --binary +1800-2017ext+sv $(TB_DIR)/$(1) $(SRC_FILES) $(CPP_FILES) --prefix $(1)  -o $(1).exe

.PHONY: compile_$(1)

run_$(1):
	./obj_dir_$(1)/$(1).exe

run_with_compile_$(1): compile_$(1)
	./obj_dir_$(1)/$(1).exe

endef

# Generate rules
$(foreach tb,$(TB_MODULES),$(eval $(call TB_RULES,$(tb))))

