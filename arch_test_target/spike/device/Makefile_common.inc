TARGET_SIM   ?= spike
TARGET_FLAGS ?= $(RISCV_TARGET_FLAGS)
ifeq ($(shell command -v $(TARGET_SIM) 2> /dev/null),)
    $(error Target simulator executable '$(TARGET_SIM)` not found)
endif

RISCV_PREFIX   ?= riscv$(XLEN)-unknown-elf-
RISCV_GCC      ?= $(RISCV_PREFIX)gcc
RISCV_OBJDUMP  ?= $(RISCV_PREFIX)objdump
RISCV_GCC_OPTS ?= -g -static -mcmodel=medany -fvisibility=hidden -nostdlib -nostartfiles $(RVTEST_DEFINES)

COMPILE_CMD = $$(RISCV_GCC) $(1) $$(RISCV_GCC_OPTS) \
							-I$(ROOTDIR)/riscv-test-suite/env/ \
							-I$(TARGETDIR)/$(RISCV_TARGET)/ \
							-T$(TARGETDIR)/$(RISCV_TARGET)/link.ld \
							$$(<) -o $$@ 
OBJ_CMD = $$(RISCV_OBJDUMP) $$@ -D > $$@.objdump; \
					$$(RISCV_OBJDUMP) $$@ --source > $$@.debug


COMPILE_TARGET=\
				$(COMPILE_CMD); \
        if [ $$$$? -ne 0 ] ; \
                then \
                echo "\e[31m$$(RISCV_GCC) failed for target $$(@) \e[39m" ; \
                exit 1 ; \
                fi ; \
				$(OBJ_CMD); \
        if [ $$$$? -ne 0 ] ; \
                then \
                echo "\e[31m $$(RISCV_OBJDUMP) failed for target $$(@) \e[39m" ; \
                exit 1 ; \
                fi ;

