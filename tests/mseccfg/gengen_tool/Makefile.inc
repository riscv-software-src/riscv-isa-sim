
ifeq ($(shell uname),Linux)
	export PLATFORM := Linux
else
ifeq ($(findstring CYGWIN,$(shell uname)),CYGWIN)
	export PLATFORM := CygWin
else #MINGW
	export PLATFORM := MinGW
endif
endif

PWD             := $(shell pwd)
gengen = ${PWD}/${dir}/tool/gengen-1.4.2/build/${PLATFORM}/bin/gengen

default:
	for skel in ${cc_skel_list}; do\
		$(gengen) -i $$skel.cc_skel --file-name $$skel.h --gen-name $$skel || exit 1; \
	done
gen:
	-rm -f ../*.c
	g++ -O2 ${cc_file}.cc -o a.out
	./a.out
	find .. -name "*.c" | wc -l

clean: $(OPT_CLEAN)
	-rm -f ../*.c 
	-rm -f a.out
	for skel in ${cc_skel_list}; do\
		rm -f $$skel.h || exit 1; \
	done
