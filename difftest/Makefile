REPO_PATH ?= ..
GUEST_ISA = riscv64

REPO_BUILD_PATH = $(REPO_PATH)/build
REPO_MAKEFILE = $(REPO_BUILD_PATH)/Makefile

$(REPO_MAKEFILE): $(REPO_PATH)/configure
	mkdir -p $(@D)
	cd $(@D) && $(abspath $(REPO_PATH))/configure --enable-dirty --with-boost=no --with-boost-asio=no --with-boost-regex=no CXXFLAGS="-O3" CFLAGS="-O3"
	sed -i -e 's/-g -O2/-O3/' $@

SPIKE = $(REPO_BUILD_PATH)/spike

$(SPIKE): $(REPO_MAKEFILE)
	$(MAKE) -C $(^D)

BUILD_DIR = ./build
$(shell mkdir -p $(BUILD_DIR))

inc_dependencies = fesvr riscv disasm customext fdt softfloat spike_main spike_dasm build
INC_PATH  = -I$(REPO_PATH) $(addprefix -I$(REPO_PATH)/, $(inc_dependencies))
INC_PATH += -I$(NEMU_HOME)/include
lib_dependencies = libspike_main.a libriscv.a libdisasm.a libsoftfloat.a libfesvr.a libfdt.a
INC_LIBS  = $(addprefix $(REPO_PATH)/build/, $(lib_dependencies))

NAME = $(GUEST_ISA)-spike-so
BINARY = $(BUILD_DIR)/$(NAME)
SRCS = difftest.cc dummy_debug.cc $(shell find ./include -name '*.h')

$(BINARY): $(SPIKE) $(SRCS)
	g++ -O3 -shared -fPIC $(INC_PATH) $(SRCS) $(INC_LIBS) -o $@

clean:
	rm -rf $(BUILD_DIR) $(REPO_BUILD_PATH)

.DEFAULT_GOAL = $(BINARY)

.PHONY: clean $(SPIKE)

