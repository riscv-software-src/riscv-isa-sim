nxpr_use = SIMM & 0x3f;
nfpr_use = (SIMM >> 6) & 0x3f;
vcfg();
setvl(RS1);
RD = VL;
