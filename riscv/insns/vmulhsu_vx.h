// vmulhsu: Signed multiply, returning high bits of product
require(STATE.VU.ELEN <= 32);
reg_t vsew = STATE.VU.vsew;
uint64_t lo_mask = (((uint64_t)1 << vsew) - 1);

VI_VX_LOOP
({
    uint64_t result = (uint64_t)rs1 * (int64_t)vs2;
    vd = (result >> sew) & lo_mask;
})
