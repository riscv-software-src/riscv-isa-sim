
EK_MACROLAR = MSTATUS_MPP MSTATUS_XS MSTATUS_FS MSTATUS_VS
EK_MACROLARI_SIFIR_TANIMLA = $(patsubst %,-D%=0,$(EK_MACROLAR))

compile-maildeki:
	riscv64-unknown-elf-gcc -march=rv64gc -mabi=lp64 -DPREALLOCATE=1 -mcmodel=medany -static -std=gnu99 -O3 -ffast-math -fno-common -fno-builtin-printf -fno-tree-loop-distribute-patterns  -o outputs/main.elf  src/main.c src/syscalls.c src/crt.S -static -nostdlib -nostartfiles -lm -lgcc -T test.ld


#-fno-common, ayni variable'in birden cok tanimi farkli .o dosyalarinda yapilmissa hata vermesine sebep oluyor.
# bunu kullanmazsak, (default) ayni variable'in farkli .o dosyalarinda yapilan tanimlari (declaration degil, definition) 
# linker tarafindan tek bir noktaya isaret edilecek sekilde cozumleniyor.

# kodda -O2, -O3, -Os option'lariyla derleme yapildiginda, 
# compiler; memset, memcmp, memcpy, strcmp, strcpy fonksiyonlarina tekabul eden pattern'lari taniyor.
# -fno-tree-loop-distribute-patterns kullanildiginda bu optimizasyonu devre disi birakiyoruz.

# -lgcc: inanilmaz bir sey. normalde default olarak kullaniliyor ama -nostdlib yapinca acik sekilde
# belirtmek gerekiyormus. 
# ozellikleri: (emin degilim, kaynak chatGPT, yanlislikla manipue etmis olabilirim)
# eger hw'de fpu gibi birimler yoksa, gnu compiler floating point variable'lari vesaire taniyip
# -lgcc'de bu islemleri integer instruction'larla yapabilen fonksiyonlarla degistiriyor.
# veya 128 bit operasyonlar.
# exception handling
# 
# 
.PHONY: compile-kirpilmis compile-ek-macrolar-sifir compile-maildeki


compile-kirpilmis:
	riscv64-unknown-elf-gcc -march=rv64gc -mabi=lp64 -static -fno-common -fno-builtin-printf -fno-tree-loop-distribute-patterns  -o outputs/main.elf  src/main.c src/syscalls.c src/crt.S -nostdlib -nostartfiles -lgcc -T test.ld

compile-ek-macrolar-sifir:
	riscv64-unknown-elf-gcc -march=rv64gc -mabi=lp64 -static -fno-common -fno-builtin-printf -fno-tree-loop-distribute-patterns  -o outputs/main.elf  src/main.c src/syscalls.c src/crt.S -nostdlib -nostartfiles -lgcc -T test.ld $(EK_MACROLARI_SIFIR_TANIMLA)
