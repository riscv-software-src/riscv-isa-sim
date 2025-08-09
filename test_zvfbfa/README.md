compile test

FAILING TESTS:
```
make test_ofp8_sat
make test_ofp8_minimal
make test_vfwcvt_sew8
```

PASSING tests:
```
make test_vfadd_simple
make test_vfsub_simple
make test_vfmax_simple
```

run test
```
spike -m0x8000:0x2000,0x28000:0x1000,0x30000:0x1000,0x38000:0x1000,0x6b000:0x1000,0x80000:0x1000,0x88000:0x1000,0x140000:0x10000,0x1700000:0x10000,0x20d0000:0x2000000,0x20000000:0x20000000,0x40000000:0x20000000,0x7f000000:0x1000000000 --isa=rv64imafdcvh_zba_zbb_zbs_zcb_zcmop_zicond_zkr_zfa_zfbfmin_zfh_zkt_zicbop_zicbom_zicboz_zicfiss_zicfilp_zimop_zawrs_zifencei_zicsr_zihintpause_zihintntl_ziccif_zicntr_zihpm_zve32f_zvl1024b_zvfbfmin_zvfbfwma_zvkt_zvfh_zvbb_zvfbfa_zvfbfmin_zvfbfwma_zvfh_zvfofp8min -l --instructions=200 test_ofp8_minimal
```