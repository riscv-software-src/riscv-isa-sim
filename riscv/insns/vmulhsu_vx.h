// vmulhsu: Signed multiply, returning high bits of product
require(STATE.VU.ELEN <= 32);
reg_t vsew = STATE.VU.vsew;
uint64_t lo_mask = (((uint64_t)1 << vsew) - 1);
uint64_t double_mask = (vsew == 32)? -1: ((uint64_t)1 << 2*vsew) - 1;
uint64_t sign_bit = ((uint64_t)1 << (vsew - 1));
uint64_t umax = -1;
uint64_t mask = 0;

VI_VX_ULOOP
({
 	mask = (vs2 & sign_bit) != 0? umax << sew :0;
 	int64_t result = (rs1 * (vs2 | mask)) & double_mask;
    vd = (result >> sew) & lo_mask;
})
