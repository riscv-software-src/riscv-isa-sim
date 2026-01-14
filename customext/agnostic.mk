customext_srcs += \
	spikema1s.cc \
	spiketa1s.cc \
	spiketama.cc \
	$(agnostic_rvv_gen_srcs) \

agnostic_riscv_insn_ext_v = \
	vadd_vv \

agnostic_rvv_gen_srcs = $(addsuffix _agnostic.cc,$(agnostic_riscv_insn_ext_v))

$(agnostic_rvv_gen_srcs): %_agnostic.cc: $(src_dir)/riscv/insns/%.h $(src_dir)/customext/spiketama_insns.cc
	sed 's/NAME/$(subst _agnostic.cc,,$@)/' $(src_dir)/customext/spiketama_insns.cc > $@

