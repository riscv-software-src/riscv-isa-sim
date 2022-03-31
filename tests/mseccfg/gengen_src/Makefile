
TOT = ..
gengen = $(TOT)/gengen_tool/gengen

default:
	rm -f test_*.h
	$(gengen) -i test_pmp_ok_1.cc_skel --file-name test_pmp_ok_1.h --gen-name pmp_ok_1
	$(gengen) -i test_pmp_csr_1.cc_skel --file-name test_pmp_csr_1.h --gen-name pmp_csr_1
	$(gengen) -i test_pmp_ok_share_1.cc_skel --file-name test_pmp_ok_share_1.h --gen-name pmp_ok_share_1
	
gen:
	-rm -rf outputs; mkdir outputs
	g++ -g3 -O2 gen_pmp_test.cc -o a.out
	./a.out
	
clean: 
	rm -rf test_*.h a.out outputs/*
