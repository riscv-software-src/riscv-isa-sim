
hello.elf:     file format elf64-littleriscv


Disassembly of section .text:

0000000080000000 <main>:
    80000000:	1101                	addi	sp,sp,-32
    80000002:	ec22                	sd	s0,24(sp)
    80000004:	1000                	addi	s0,sp,32
    80000006:	fe042623          	sw	zero,-20(s0)
    8000000a:	0001                	nop
    8000000c:	6462                	ld	s0,24(sp)
    8000000e:	6105                	addi	sp,sp,32
    80000010:	8082                	ret

Disassembly of section .bss:

0000000080001000 <tohost>:
	...

00000000800013e8 <fromhost>:
	...

Disassembly of section .comment:

0000000000000000 <.comment>:
   0:	3a434347          	fmsub.d	ft6,ft6,ft4,ft7,rmm
   4:	2820                	fld	fs0,80(s0)
   6:	65653267          	.4byte	0x65653267
   a:	6535                	lui	a0,0xd
   c:	3334                	fld	fa3,96(a4)
   e:	3030                	fld	fa2,96(s0)
  10:	3831                	addiw	a6,a6,-20
  12:	2029                	.2byte	0x2029
  14:	3231                	addiw	tp,tp,-20 # ffffffffffffffec <_end+0xffffffff7fffe81c>
  16:	322e                	fld	ft4,232(sp)
  18:	302e                	fld	ft0,232(sp)
	...

Disassembly of section .riscv.attributes:

0000000000000000 <.riscv.attributes>:
   0:	3441                	addiw	s0,s0,-16
   2:	0000                	unimp
   4:	7200                	ld	s0,32(a2)
   6:	7369                	lui	t1,0xffffa
   8:	01007663          	bgeu	zero,a6,14 <main-0x7fffffec>
   c:	002a                	c.slli	zero,0xa
   e:	0000                	unimp
  10:	1004                	addi	s1,sp,32
  12:	7205                	lui	tp,0xfffe1
  14:	3676                	fld	fa2,376(sp)
  16:	6934                	ld	a3,80(a0)
  18:	7032                	.2byte	0x7032
  1a:	5f30                	lw	a2,120(a4)
  1c:	326d                	addiw	tp,tp,-5 # fffffffffffe0ffb <_end+0xffffffff7ffdf82b>
  1e:	3070                	fld	fa2,224(s0)
  20:	615f 7032 5f30      	.byte	0x5f, 0x61, 0x32, 0x70, 0x30, 0x5f
  26:	3266                	fld	ft4,120(sp)
  28:	3070                	fld	fa2,224(s0)
  2a:	645f 7032 5f30      	.byte	0x5f, 0x64, 0x32, 0x70, 0x30, 0x5f
  30:	30703263          	.4byte	0x30703263
	...
