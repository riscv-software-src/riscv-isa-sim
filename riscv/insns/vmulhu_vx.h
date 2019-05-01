// vmulhu: Unsigned multiply, returning high bits of product
require(p->VU.ELEN <= 32);
reg_t vsew = p->VU.vsew;
uint64_t lo_mask = (((uint64_t)1 << vsew) - 1);

VI_VX_ULOOP
({
    uint64_t result = (uint64_t)rs1 * (uint64_t)vs2;
    vd = (result >> sew) & lo_mask;
})
