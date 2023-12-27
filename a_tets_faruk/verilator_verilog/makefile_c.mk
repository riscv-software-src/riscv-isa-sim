.PHONY: all clean $(MAIN_TARGETS)

# Directories
SRC_DIR := src
INC_DIR := inc
ENTRY_POINTS_DIR := entry_points
TEMP_DIR := temp
OUTPUT_DIR := outputs

# Compiler and flags
CC := gcc
# g flag: add debug info
CFLAGS := -I$(INC_DIR) -Wall -g

# Automatically detect main files
MAIN_FILES := $(wildcard $(ENTRY_POINTS_DIR)/*.c)
MAIN_TARGETS := $(patsubst $(ENTRY_POINTS_DIR)/%.c,$(OUTPUT_DIR)/%,$(MAIN_FILES))

# Source files and object files
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(TEMP_DIR)/%.o,$(SRC_FILES))

# Default target
all: $(MAIN_TARGETS)

# Clean rule
clean:
	rm -rf $(TEMP_DIR) $(OUTPUT_DIR)

# Create output and temp directories
$(TEMP_DIR) $(OUTPUT_DIR):
	mkdir -p $@

# faruk:
# .d dosyalari gnu compiler tarafindan dinamik olarak olusturuluyor.
# ilk compile ettigimizde .d dosyasi olmadigindan 
# butun dosyalar compile ediliyor.
# sonraki derlemelerde .d dosyalari kullanilarak dependency'lerin
# gnu compiler tarafindan cozulmesini sagliyor.
# bir .d dosyasi ornegi:
# add_uw.o: add_uw.cc ../riscv/insn_macros.h ../riscv/insns/add_uw.h
# ../riscv/insn_macros.h:
# ../riscv/insns/add_uw.h:
# bu asagidaki include statement, d dosyalarinin icerigini
# bu makefile scriptine kopyaliyor. ~faruk
# Include dynamically generated dependencies
-include $(OBJ_FILES:.o=.d)

# faruk: gnu compiler'a dependency dosyasi olusturtmak icin. ~faruk
# Rule to generate dependencies
$(TEMP_DIR)/%.d: $(SRC_DIR)/%.c | $(TEMP_DIR)
	$(CC) $(CFLAGS) -MM -MT $(@:.d=.o) $< > $@

# Generic rule for object files
$(TEMP_DIR)/%.o: $(SRC_DIR)/%.c | $(TEMP_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Generate rules for each main target
define MAIN_RULE
$(1): $(ENTRY_POINTS_DIR)/$$(notdir $$(basename $(1))).c $(OBJ_FILES) | $(OUTPUT_DIR)
	$(CC) $(CFLAGS) $$^ -o $$@
endef

$(foreach target,$(MAIN_TARGETS),$(eval $(call MAIN_RULE,$(target))))
