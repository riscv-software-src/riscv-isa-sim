SRC_DIR = ..
BUILD_DIR = ./build
ORI_MAKE_DIR  = ../build/
inc_dependencies = fesvr riscv disasm customext fdt softfloat spike_main spike_dasm build
INC_DIR   = -I. -I$(SRC_DIR) $(addprefix -I$(SRC_DIR)/, $(inc_dependencies))
lib_dependencies = libspike_main.a  libriscv.a  libdisasm.a  libsoftfloat.a  libfesvr.a  libfdt.a
INC_LIBS  = $(addprefix $(SRC_DIR)/build/, $(lib_dependencies))

DIFF_BINARY = difftest
DIFF_SRC = difftest.cc
DIFF_SO  = difftest.so
TEST_BINARY = test
TEST_SRC = test.cc
EXEC = $(BUILD_DIR)/$(TEST_BINARY)

CFLAGS = -O2 -MMD -ggdb3
SO_FLAGS = -shared -fPIC

$(shell mkdir -p $(BUILD_DIR))

difftest:
	make -C $(ORI_MAKE_DIR)
	g++ $(CFLAGS) $(INC_DIR) $(DIFF_SRC) $(INC_LIBS) $(SO_FLAGS) -o $(BUILD_DIR)/$(DIFF_SO)
	g++ $(TEST_SRC) -ldl -o $(BUILD_DIR)/$(TEST_BINARY)

test:
	$(EXEC)

clean:
	rm -rf $(BUILD_DIR)


.PHONY: difftest test clean