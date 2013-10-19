require_supervisor_hwacha;
reg_t addr = XS1;

#define LOAD_B(addr) \
  (addr += 1, p->get_mmu()->load_uint8(addr-1))

#define LOAD_W(addr) \
  (addr += 4, p->get_mmu()->load_uint32(addr-4))

#define LOAD_D(addr) \
  (addr += 8, p->get_mmu()->load_uint64(addr-8))


WRITE_NXPR(LOAD_W(addr));
WRITE_NFPR(LOAD_W(addr));
WRITE_MAXVL(LOAD_W(addr));
WRITE_VL(LOAD_W(addr));
WRITE_UTIDX(LOAD_W(addr));
addr += 4;
WRITE_VF_PC(LOAD_D(addr));

for (uint32_t x=1; x<NXPR; x++) {
  for (uint32_t i=0; i<VL; i++) {
    UT_WRITE_XPR(i, x, LOAD_D(addr));
  }
}

for (uint32_t f=0; f<NFPR; f++) {
  for (uint32_t i=0; i<VL; i++) {
    UT_WRITE_FPR(i, f, LOAD_D(addr));
  }
}

for (uint32_t i=0; i<VL; i++) {
  h->get_ut_state(i)->run = LOAD_B(addr);
}

#undef LOAD_B
#undef LOAD_W
#undef LOAD_D
