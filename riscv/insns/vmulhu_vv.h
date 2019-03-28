// vmulhu: Unsigned multiply, returning high bits of product
require(STATE.VU.ELEN <= 32);
reg_t vsew = STATE.VU.vsew;
uint64_t sew_result_bits = vsew * 2;
uint64_t upper_mask = 1 - ((1 << vsew) - 1);

VI_VV_LOOP
({
    uint64_t result = vs1 * vs2;
    vd = (result>>(sew_result_bits - vsew)) & upper_mask;
})
